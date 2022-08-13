#include "Patches/HookLevelCollectionTableSet.hpp"
#include "Patches/HookSelectedCollection.hpp"
#include "Patches/HookSelectedCategory.hpp"
#include "Patches/HookSelectedInTable.hpp"
#include "Utils/PlaylistUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

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

namespace BetterSongList::Hooks {
    ISorter* HookLevelCollectionTableSet::sorter;
    IFilter* HookLevelCollectionTableSet::filter;
    std::function<void(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>)> HookLevelCollectionTableSet::recallLast;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::lastInMapList;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::lastOutMapList;
    SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> HookLevelCollectionTableSet::asyncPreProcessed;
    std::vector<HookLevelCollectionTableSet::CustomLegendPair> HookLevelCollectionTableSet::customLegend;
    bool HookLevelCollectionTableSet::prepareThreadCurrentlyRunning = false;

    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> HookLevelCollectionTableSet::get_lastInMapList() {
        if (!lastInMapList) {
            lastInMapList.emplace(Array<GlobalNamespace::IPreviewBeatmapLevel*>::NewLength(0));
        }
        return lastInMapList.ptr();
    }

    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> HookLevelCollectionTableSet::get_lastOutMapList() {
        if (!lastOutMapList) {
            lastOutMapList.emplace(Array<GlobalNamespace::IPreviewBeatmapLevel*>::NewLength(0));
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
        if (!(filter ? filter->get_isReady() : false) && !(sorter ? sorter->get_isReady() : false)) return;

        std::stringstream threadId;
        threadId << std::this_thread::get_id();
        DEBUG("FilterWrapper({}) - thread {}", previewBeatmapLevels.size(), threadId.str());
        
        std::vector<GlobalNamespace::IPreviewBeatmapLevel*> out;
        if (filter ? filter->get_isReady() : false) {
            // check every level if they match the current filter
            for (auto level : previewBeatmapLevels) {
                if (filter->GetValueFor(level)) {
                    out.push_back(level);
                }
            }
        }

        previewBeatmapLevels = ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>(out.size());
        memcpy(previewBeatmapLevels.begin(), out.data(), out.size());

        if (sorter ? sorter->get_isReady() : false) {
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
                castedSorter.customSorter->DoSort(previewBeatmapLevels, config.sortAscending);
            } 
            // this checks if the server is an ISorterPrimitive with rtti
            else if (sorter && sorter->as<ISorterPrimitive*>()) {
                if (config.sortAscending) {
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
            auto legendMap = withLegend->BuildLegend(previewBeatmapLevels);
            for (const auto& [key, value] : legendMap) {
                customLegend.emplace_back(key, value);
            }
        }
    }

    bool HookLevelCollectionTableSet::PrepareStuffIfNecessary(std::function<void()> cb, bool cbOnAlreadyPrepared) {
        using namespace System::Threading::Tasks;
        if ((filter && !filter->get_isReady()) || (sorter && !sorter->get_isReady())) {
            
            auto instance = FilterUI::get_instance();
            auto indicator = instance->filterLoadingIndicator;
            if (indicator && indicator->m_CachedPtr.m_value) {
                indicator->get_gameObject()->SetActive(true);
            }

            DEBUG("PrepareStuffIfNecessary()");
            if (!prepareThreadCurrentlyRunning) {
                prepareThreadCurrentlyRunning = true;
                std::thread([cb]() {
                    if (sorter && !sorter->get_isReady()) sorter->Prepare(); 
                    if (filter && !filter->get_isReady()) filter->Prepare(); 

                    DEBUG("Sorter / Filter prepared");

                    prepareThreadCurrentlyRunning = false;
                    if (cb) cb();
                }).detach();
            }
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
            self->SetData((System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IPreviewBeatmapLevel*>*)data, favoriteLevelIds, isSorted);
        };

        if (!sorter || sorter->get_isReady()) {
            beatmapLevelsAreSorted = false;
            DEBUG("We can Sort!");
        }

        if (PrepareStuffIfNecessary([](){Refresh(true);})) {
            DEBUG("Stuff isnt ready yet... Preparing it and then reloading list: Sorter {0}, Filter {1}", sorter ? sorter->get_isReady() : false, filter ? filter->get_isReady() : false);
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
        if (customLegend.empty()) return;

        auto alphabetScrollBar = self->alphabetScrollbar;
        auto data = ArrayW<GlobalNamespace::AlphabetScrollInfo::Data*>(il2cpp_array_size_t(customLegend.size()));
        int i = 0;
        for (const auto& [key, value] : customLegend)
            data[i] = GlobalNamespace::AlphabetScrollInfo::Data::New_ctor(u'?', value);
        alphabetScrollBar->SetData(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::AlphabetScrollInfo::Data*>*>(data.convert()));

        ListWrapper<TMPro::TextMeshProUGUI*> texts{alphabetScrollBar->texts};
        for (int i = 0; i < texts.size(); i++) {
            texts[i]->set_text(customLegend[i].key);
        }
        customLegend.clear();
        auto tableViewT = reinterpret_cast<UnityEngine::RectTransform*>(self->tableView->get_transform());
        auto scrollBarT = reinterpret_cast<UnityEngine::RectTransform*>(alphabetScrollBar->get_transform());
        tableViewT->set_offsetMin({scrollBarT->get_rect().get_size().x + 1.0f, 0.0f});
        alphabetScrollBar->get_gameObject()->SetActive(true);
    }
}