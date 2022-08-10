#include "Patches/RestoreLevelSelection.hpp"
#include "Patches/RestoreTableScroll.hpp"
#include "Patches/NavigationRestorePrepare.hpp"
#include "Patches/HookLevelCollectionTableSet.hpp"

#include "UnityEngine/GameObject.hpp"

#include "logging.hpp"
#include "config.hpp"

namespace BetterSongList::Hooks {
    void RestoreLevelSelection::LevelFilteringNavigationController_ShowPacksInSecondChildController_Prefix(StringW& levelPackIdToBeSelectedAfterPresent) {
        if (Il2CppString::IsNullOrEmpty(levelPackIdToBeSelectedAfterPresent)) {
            levelPackIdToBeSelectedAfterPresent = NavigationRestorePrepare::get_collection();
        }
    }

    bool RestoreLevelSelection::LevelCollectionTableView_SelectLevel_Prefix(System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IPreviewBeatmapLevel*>* previewBeatmapLevels) {
        return previewBeatmapLevels;
    }

    void RestoreLevelSelection::LevelCollectionNavigationController_DidActivate_Prefix(GlobalNamespace::LevelCollectionNavigationController* self, bool addedToHierarchy, GlobalNamespace::IPreviewBeatmapLevel*& beatmapLevelToBeSelectedAfterPresent) {
        auto restoreTo = BetterSongList::Hooks::NavigationRestorePrepare::get_level();
        if (!addedToHierarchy || self->beatmapLevelToBeSelectedAfterPresent || !config.reselectLastSong || restoreTo.empty()) {
            return;
        }

        auto mapList = BetterSongList::Hooks::HookLevelCollectionTableSet::get_lastOutMapList();
        self->beatmapLevelToBeSelectedAfterPresent = mapList ? mapList.FirstOrDefault([&restoreTo](auto level) {
            return level->get_levelID() == restoreTo;
        }) : nullptr;
    }

    void RestoreLevelSelection::LevelCollectionNavigationController_DidActivate_Postfix(bool addedToHierarchy, bool& hideDetailViewController) {
        RestoreTableScroll::GotoLastSelectedOnNextSetData();
        if (addedToHierarchy) hideDetailViewController = false;
    }

    void RestoreLevelSelection::SelectLevelCategoryViewController_Setup_Prefix(GlobalNamespace::SelectLevelCategoryViewController::LevelCategory& selectedCategory) {
        auto restoreTo = NavigationRestorePrepare::get_category();

        if (restoreTo != GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None)
            selectedCategory = restoreTo;
    }

    bool RestoreLevelSelection::LevelPackDetailViewController_RefreshAvailabilityAsync_Prefix(GlobalNamespace::LevelPackDetailViewController* self, GlobalNamespace::IBeatmapLevelPack*& pack) {
        if (!pack) {
            // TODO: get custom levels pack from songloader
            // pack = songcore.customlevelspack
        }

        if (!pack) {
            self->get_gameObject()->SetActive(false);
            return false;
        }

        return true;
    }
}