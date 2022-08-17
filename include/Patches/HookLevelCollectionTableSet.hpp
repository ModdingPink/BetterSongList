#pragma once

#include "Sorters/Models/ISorter.hpp"
#include "Filters/Models/IFilter.hpp"

#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include <thread>

namespace BetterSongList::Hooks {
    template<typename T>
    using HashSet = System::Collections::Generic::HashSet_1<T>;
    class HookLevelCollectionTableSet {
        public:
            static ISorter* sorter;
            static IFilter* filter;

            static ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> get_lastInMapList();
            static ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> get_lastOutMapList();

            static void Refresh(bool processAsync = false, bool clearAsyncResult = true);

            /// @brief prio int max value
            static void LevelCollectionTableView_SetData_Prefix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& previewBeatmapLevels, HashSet<StringW>* favoriteLevelIds, bool& beatmapLevelsAreSorted);
            /// @brief no prio
            static void LevelCollectionTableView_SetData_PostFix(GlobalNamespace::LevelCollectionTableView* self, ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> previewBeatmapLevels);
        private:
            static void FilterWrapper(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& previewBeatmapLevels);
            static bool PrepareStuffIfNecessary(std::function<void()> cb = nullptr, bool cbOnAlreadyPrepared = false);
            static std::function<void(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>)> recallLast;
            static SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> lastInMapList;
            static SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> lastOutMapList;
            static SafePtr<Array<GlobalNamespace::IPreviewBeatmapLevel*>> asyncPreProcessed;
            static ISorterWithLegend::Legend customLegend;
            static bool prepareThreadCurrentlyRunning;
    };
}