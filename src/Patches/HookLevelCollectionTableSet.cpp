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
        
        std::vector<GlobalNamespace::IPreviewBeatmapLevel*> out;
        if (filter && filter->get_isReady()) {
            INFO("Filtering levels");
            // yes this is way too much space, but worst case all levels match the filter
            // also reserving space beforehand is better than allocating it every push back
            out.reserve(previewBeatmapLevels.Length());
            // check every level if they match the current filter
            for (auto level : previewBeatmapLevels) {
                if (filter->GetValueFor(level)) {
                    out.push_back(level);
                }
            }
        }

        // if we filtered anything, no filter means no change in contents
        // also if all levels match the filter, we do not need to make a new array
        if (!out.empty() && out.size() != previewBeatmapLevels.Length()) {
            previewBeatmapLevels = ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>(out.size());
            memcpy(previewBeatmapLevels.begin(), out.data(), out.size());
        }

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
                castedSorter.customSorter->DoSort(previewBeatmapLevels, config.sortAsc);
            } 
            // this checks if the server is an ISorterPrimitive with rtti
            else if (sorter && sorter->as<ISorterPrimitive*>()) {
                if (config.sortAsc) {
                    std::sort(previewBeatmapLevels.begin(), previewBeatmapLevels.end(), 
                        [primitiveSorter = castedSorter.primitiveSorter](GlobalNamespace::IPreviewBeatmapLevel* lhs, GlobalNamespace::IPreviewBeatmapLevel* rhs) -> bool {
                            return primitiveSorter->GetValueFor(lhs).value_or(std::numeric_limits<float>::infinity()) <
                            primitiveSorter->GetValueFor(rhs).value_or(-std::numeric_limits<float>::infinity());
                    });
                } else {
                    std::sort(previewBeatmapLevels.begin(), previewBeatmapLevels.end(), 
                        [primitiveSorter = castedSorter.primitiveSorter](GlobalNamespace::IPreviewBeatmapLevel* lhs, GlobalNamespace::IPreviewBeatmapLevel* rhs) -> bool {
                            return primitiveSorter->GetValueFor(rhs).value_or(std::numeric_limits<float>::infinity()) <
                            primitiveSorter->GetValueFor(lhs).value_or(-std::numeric_limits<float>::infinity());
                    });
                }
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
            customLegend = withLegend->BuildLegend(previewBeatmapLevels);
        }

        INFO("Ended with {} levels in array", previewBeatmapLevels.size());
    }


    static SafePtr<System::Threading::CancellationTokenSource> sortCancelSrc;

    bool HookLevelCollectionTableSet::PrepareStuffIfNecessary(std::function<void()> cb, bool cbOnAlreadyPrepared) {
        INFO("PrepareStuffIfNecessary({}, {})", cb != nullptr, cbOnAlreadyPrepared);

        using namespace System::Threading::Tasks;
        if ((filter && !filter->get_isReady()) || (sorter && !sorter->get_isReady())) {
            auto instance = FilterUI::get_instance();
            auto indicator = instance->filterLoadingIndicator;
            if (indicator && indicator->m_CachedPtr.m_value) {
                indicator->get_gameObject()->SetActive(true);
            }
            if (sortCancelSrc) {
                sortCancelSrc->Cancel();
            }

            sortCancelSrc = System::Threading::CancellationTokenSource::New_ctor();
            auto thisSrc = sortCancelSrc.ptr();

            DEBUG("PrepareStuffIfNecessary()");
            ArrayW<Task*> tasks = ArrayW<Task*>(il2cpp_array_size_t(2));
            tasks[0] = (sorter && !sorter->get_isReady()) ? sorter->Prepare() : Task::get_CompletedTask();
            tasks[1] = (filter && !filter->get_isReady()) ? filter->Prepare() : Task::get_CompletedTask();

            std::function<void(Task*)> fun =
                [cb, thisSrc](auto task) { 

                    DEBUG("ContinueWith");
                    if (sortCancelSrc.ptr() != thisSrc) {
                        return;
                    }

                    sortCancelSrc.emplace(nullptr);
                    if (!thisSrc->get_IsCancellationRequested() && cb) {
                        cb();
                    }
                };

            System::Action_1<Task*>* continueAction = BSML::MakeSystemAction(fun);
            System::Threading::StackCrawlMark stackCrawlMark = System::Threading::StackCrawlMark::LookForMyCaller;

            auto task = Task::WhenAll(tasks)->ContinueWith(
                continueAction,
                System::Threading::Tasks::TaskScheduler::get_Current(),
                System::Threading::CancellationToken::get_None(),
                System::Threading::Tasks::TaskContinuationOptions::OnlyOnRanToCompletion,
                byref(stackCrawlMark)
            );
            task->Execute();

            return true;
        } else {
            INFO("Sorter & Filter were prepared!");
        }

        if (cbOnAlreadyPrepared && cb) cb();
        return false;
    }

    void HookLevelCollectionTableSet::Prefix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& previewBeatmapLevels, HashSet<StringW>* favoriteLevelIds, bool& beatmapLevelsAreSorted) {
        DEBUG("LevelCollectionTableView.SetData() : Prefix");
        if (lastInMapList && previewBeatmapLevels.convert() == lastInMapList.ptr()) {
            DEBUG("LevelCollectionTableView.SetData() : Prefix -> levels = lastout because lastin == levels");
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

    void HookLevelCollectionTableSet::PostFix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> previewBeatmapLevels) {
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