#pragma once

#include "GlobalNamespace/AnnotatedBeatmapLevelCollectionsViewController.hpp"
#include "GlobalNamespace/IAnnotatedBeatmapLevelCollection.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

namespace BetterSongList::Hooks {
    class HookSelectedCollection {
        public:
            static GlobalNamespace::IAnnotatedBeatmapLevelCollection* get_lastSelectedCollection();
            static void AnnotatedBeatmapLevelCollectionsViewController_HandleDidSelectAnnotatedBeatmapLevelCollection_Prefix(GlobalNamespace::IAnnotatedBeatmapLevelCollection* beatmapLevelCollection);
            static void AnnotatedBeatmapLevelCollectionsViewController_SetData_PostFix(GlobalNamespace::AnnotatedBeatmapLevelCollectionsViewController* self);
            static void LevelFilteringNavigationController_UpdateSecondChildControllerContent_Prefix(GlobalNamespace::SelectLevelCategoryViewController::LevelCategory levelCategory);
        private:
            static SafePtr<GlobalNamespace::IAnnotatedBeatmapLevelCollection> lastSelectedCollection;
    };
}