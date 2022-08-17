#include "Patches/HookLevelCollectionTableSet.hpp"
#include "Patches/HookSelectedCollection.hpp"
#include "Patches/HookSelectedCategory.hpp"
#include "Patches/HookSelectedInTable.hpp"
#include "Utils/PlaylistUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/TaskScheduler.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "System/Threading/CancellationTokenSource.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "HMUI/AlphabetScrollbar.hpp"
#include "GlobalNamespace/AlphabetScrollInfo.hpp"
#include "GlobalNamespace/AlphabetScrollInfo_Data.hpp"

#include "sombrero/shared/linq_functional.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "UI/FilterUI.hpp"
#include <cxxabi.h>
#include <sstream>
#include <future>


namespace BetterSongList::Hooks {
    ISorter* HookLevelCollectionTableSet::sorter;
    IFilter* HookLevelCollectionTableSet::filter;
    std::function<void(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>)> HookLevelCollectionTableSet::recallLast;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::lastInMapList;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::lastOutMapList;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::asyncPreProcessed;
    ISorterWithLegend::Legend HookLevelCollectionTableSet::customLegend;
    bool HookLevelCollectionTableSet::prepareThreadCurrentlyRunning = false;

    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> HookLevelCollectionTableSet::get_lastInMapList() {
        if (!lastInMapList) {
            return nullptr;
        }
        return lastInMapList.ptr();
    }

    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> HookLevelCollectionTableSet::get_lastOutMapList() {
        if (!lastOutMapList) {
            return nullptr;
        }
        return lastOutMapList.ptr();
    }

    void HookLevelCollectionTableSet::Refresh(bool processAsync, bool clearAsyncResult) {
        if (!lastInMapList) {
            return;
        }

        DEBUG("Refresh({}, {})", processAsync, clearAsyncResult);

        if (clearAsyncResult) {
            asyncPreProcessed.emplace(nullptr);
        }

        if (processAsync) {
            PrepareStuffIfNecessary([](){
                auto inList = get_lastInMapList();
                FilterWrapper(inList);
                asyncPreProcessed.emplace(static_cast<Array<GlobalNamespace::IPreviewBeatmapLevel*>*>(inList));
                Refresh(false, false);
            }, true);
            return;
        }

        auto ml = get_lastInMapList();
        lastInMapList.emplace(nullptr);
        if (recallLast) recallLast(ml);
    }

    void HookLevelCollectionTableSet::FilterWrapper(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& previewBeatmapLevels) {
        if (!previewBeatmapLevels) {
            return;
        }
        
        if ((filter && !filter->get_isReady()) && (sorter && !sorter->get_isReady())) return;

        DEBUG("FilterWrapper({})", previewBeatmapLevels.size());
        
        using namespace Sombrero::Linq::Functional;
        if (filter && filter->get_isReady()) {
            INFO("Filtering levels");
            previewBeatmapLevels = previewBeatmapLevels | Where([filter = filter](auto x){ return filter->GetValueFor(x); }) | ToArray();
        }

        INFO("We're down to {}", previewBeatmapLevels.size());
        if (sorter && sorter->get_isReady()) {
            INFO("Sorting levels!");
            // you say RED WTF IS GOING ON HERE
            union {
                ISorter* sorter;
                ISorterCustom* customSorter;
                ISorterPrimitive* primitiveSorter;
            } castedSorter;
            // this union just holds different types, but really it's 1 pointer, which is assigned here
            castedSorter.sorter = sorter;

            // this checks if the server is an ISorterCustom with rtti
            if (sorter && sorter->as<ISorterCustom*>()) {
                castedSorter.customSorter->DoSort(previewBeatmapLevels, config.get_sortAsc());
            } 
            // this checks if the server is an ISorterPrimitive with rtti
            else if (sorter && sorter->as<ISorterPrimitive*>()) {
                // sorting is the same regardless of ascending or descending, since the way we differentiate between ascending and descending is to just use the reverse iterators if ascending
                auto sort = [primitiveSorter = castedSorter.primitiveSorter]
                (GlobalNamespace::IPreviewBeatmapLevel* lhs, GlobalNamespace::IPreviewBeatmapLevel* rhs) -> bool {
                    return primitiveSorter->GetValueFor(lhs) < primitiveSorter->GetValueFor(rhs);
                };

                if (config.get_sortAsc()) std::sort(previewBeatmapLevels.rbegin(), previewBeatmapLevels.rend(), sort);
                else std::sort(previewBeatmapLevels.begin(), previewBeatmapLevels.end(), sort);
            } 
            // if it was neither, print the type name
            else if (sorter) {
                ISorter& s = *sorter;
                auto& ti = typeid(s);
                int status;
                auto realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
                ERROR("Sorter was of type {} which is not a valid sorter type!", realname);
                free(realname);
            } else {
                DEBUG("Sorter was null!");
            }
        }

        auto withLegend = sorter ? sorter->as<ISorterWithLegend*>() : nullptr;
        if (withLegend) {
            INFO("Sorter had legend!");
            customLegend = withLegend->BuildLegend(previewBeatmapLevels);
            for (auto const& [first, second] : customLegend) {
                INFO("{} : {}", first, second);
            }
        } else {
            ERROR("Sorter did not have legend!");
        }

        INFO("Ended with {} levels in array", previewBeatmapLevels.size());
    }


    static std::shared_ptr<bool> doCancelSort;

    bool HookLevelCollectionTableSet::PrepareStuffIfNecessary(std::function<void()> cb, bool cbOnAlreadyPrepared) {
        INFO("PrepareStuffIfNecessary({}, {})", cb != nullptr, cbOnAlreadyPrepared);

        if ((filter && !filter->get_isReady()) || (sorter && !sorter->get_isReady())) {
            auto instance = FilterUI::get_instance();
            auto indicator = instance->filterLoadingIndicator;
            if (indicator && indicator->m_CachedPtr.m_value) {
                indicator->get_gameObject()->SetActive(true);
            }

            if (doCancelSort.use_count() > 0) {
                *doCancelSort.get() = true;
                doCancelSort.reset();
            }

            doCancelSort = std::make_shared<bool>(false);
            DEBUG("PrepareStuffIfNecessary()");
            std::thread([cb](std::weak_ptr<bool> thisDoCancelSort){
                if (sorter && !sorter->get_isReady()) { 
                    sorter->Prepare().wait();
                }

                if (filter && !filter->get_isReady()) { 
                    filter->Prepare().wait();
                }

                DEBUG("ContinueWith");
                if (thisDoCancelSort.expired()) {
                    INFO("sort was cancelled by invalidated weak_ptr");
                    return;
                }

                if (!*thisDoCancelSort.lock().get() && cb) {
                    // main thread because cb possibly does main thread things
                    QuestUI::MainThreadScheduler::Schedule(cb);
                }
            }, doCancelSort).detach();

            return true;
        } else {
            INFO("Sorter & Filter were prepared!");
        }

        if (cbOnAlreadyPrepared && cb) cb();
        return false;
    }

    void HookLevelCollectionTableSet::LevelCollectionTableView_SetData_Prefix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& previewBeatmapLevels, HashSet<StringW>* favoriteLevelIds, bool& beatmapLevelsAreSorted) {
        DEBUG("LevelCollectionTableView.SetData() : Prefix");
        if (lastInMapList && previewBeatmapLevels.convert() == lastInMapList.ptr() && lastOutMapList) {
            DEBUG("LevelCollectionTableView.SetData() : Prefix -> levels = lastout because {} == {}", fmt::ptr(previewBeatmapLevels.convert()), fmt::ptr(lastInMapList.ptr()));
            previewBeatmapLevels = lastOutMapList.ptr();
            return;
        }

        if (HookSelectedCollection::get_lastSelectedCollection() && PlaylistUtils::get_hasPlaylistLib()) {
            auto playlistArr = PlaylistUtils::GetLevelsForLevelCollection(HookSelectedCollection::get_lastSelectedCollection());
            if (playlistArr) {
                previewBeatmapLevels = playlistArr;
            }
        }

        lastInMapList.emplace(static_cast<Array<GlobalNamespace::IPreviewBeatmapLevel*>*>(previewBeatmapLevels));
        auto isSorted = beatmapLevelsAreSorted;
        recallLast = [self, favoriteLevelIds, isSorted](ArrayW<GlobalNamespace::IPreviewBeatmapLevel *> overrideData){
            auto data = overrideData ? static_cast<Array<GlobalNamespace::IPreviewBeatmapLevel*>*>(overrideData) : lastInMapList.ptr();
            INFO("recallLast, Data: {}", fmt::ptr(data));
            if (data) {
                INFO("Setting data with {} levels", data->Length());
            }
            self->SetData((System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IPreviewBeatmapLevel*>*)data, favoriteLevelIds, isSorted);
        };

        if (!sorter || sorter->get_isReady()) {
            beatmapLevelsAreSorted = false;
            DEBUG("We have to sort!");
        }

        if (PrepareStuffIfNecessary([](){Refresh(true);})) {
            DEBUG("Stuff isnt ready yet... Preparing it and then reloading list: Sorter {0}, Filter {1}", !sorter || sorter->get_isReady(), !filter || filter->get_isReady());
        }

        auto instance = FilterUI::get_instance();
        auto loadingIndicator = instance->filterLoadingIndicator;
        if (loadingIndicator && loadingIndicator->m_CachedPtr.m_value) {
            loadingIndicator->get_gameObject()->SetActive(false);
        }

        if (asyncPreProcessed) {
            previewBeatmapLevels = asyncPreProcessed.ptr();
			DEBUG("Used Async-Prefiltered");
            asyncPreProcessed.emplace(nullptr);
            return;
        }
        
        FilterWrapper(previewBeatmapLevels);
    }

    void HookLevelCollectionTableSet::LevelCollectionTableView_SetData_PostFix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> previewBeatmapLevels) {
        lastOutMapList.emplace(static_cast<Array<GlobalNamespace::IPreviewBeatmapLevel*>*>(previewBeatmapLevels));
        DEBUG("HookLevelCollectionTableSet::PostFix({}, {})", fmt::ptr(self), previewBeatmapLevels.size());
        if (customLegend.empty()) return;
        auto alphabetScrollBar = self->alphabetScrollbar;
        auto data = ArrayW<GlobalNamespace::AlphabetScrollInfo::Data*>(il2cpp_array_size_t(customLegend.size()));
        DEBUG("Legend size: {}, {}", data->Length(), customLegend.size());
        for (int i = 0; const auto& [key, value] : customLegend)
            data[i++] = GlobalNamespace::AlphabetScrollInfo::Data::New_ctor(u'?', value);
        DEBUG("Setting data");
        alphabetScrollBar->SetData(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::AlphabetScrollInfo::Data*>*>(data.convert()));

        DEBUG("making texts");
        ListWrapper<TMPro::TextMeshProUGUI*> texts{alphabetScrollBar->texts};
        DEBUG("setting values");
        for (int i = 0; const auto& [key, value] : customLegend) {
            DEBUG("{}: {}", i, key);
            texts[i++]->set_text(key);
        }

        DEBUG("Legend Clear");
        customLegend.clear();
        auto tableViewT = reinterpret_cast<UnityEngine::RectTransform*>(self->tableView->get_transform());
        auto scrollBarT = reinterpret_cast<UnityEngine::RectTransform*>(alphabetScrollBar->get_transform());
        tableViewT->set_offsetMin({scrollBarT->get_rect().get_size().x + 1.0f, 0.0f});
        alphabetScrollBar->get_gameObject()->SetActive(true);
        DEBUG("scroll bar active!");
    }
}