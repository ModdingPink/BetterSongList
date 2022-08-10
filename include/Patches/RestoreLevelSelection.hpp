#pragma once

#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"
#include "GlobalNamespace/LevelPackDetailViewController.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"

namespace BetterSongList::Hooks {
    class RestoreLevelSelection {
        public:
            static void LevelFilteringNavigationController_ShowPacksInSecondChildController_Prefix(StringW& levelPackIdToBeSelectedAfterPresent);
            static bool LevelCollectionTableView_SelectLevel_Prefix(System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IPreviewBeatmapLevel*>* previewBeatmapLevels);
            static void LevelCollectionNavigationController_DidActivate_Prefix(GlobalNamespace::LevelCollectionNavigationController* self, bool addedToHierarchy, GlobalNamespace::IPreviewBeatmapLevel*& beatmapLevelToBeSelectedAfterPresent);
            static void LevelCollectionNavigationController_DidActivate_Postfix(bool addedToHierarchy, bool& hideDetailViewController);
            static void SelectLevelCategoryViewController_Setup_Prefix(GlobalNamespace::SelectLevelCategoryViewController::LevelCategory& selectedCategory);
            static bool LevelPackDetailViewController_RefreshAvailabilityAsync_Prefix(GlobalNamespace::LevelPackDetailViewController* self, GlobalNamespace::IBeatmapLevelPack*& pack);
    };
}