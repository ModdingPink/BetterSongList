#include "hooking.hpp"
#include "Patches/HookSelectedCollection.hpp"
#include "Patches/NavigationRestorePrepare.hpp"

#include "GlobalNamespace/LevelFilteringNavigationController.hpp"

// from RestoreLevelSelection
MAKE_AUTO_HOOK_MATCH(LevelFilteringNavigationController_ShowPacksInSecondChildController, &GlobalNamespace::LevelFilteringNavigationController::ShowPacksInSecondChildController, void, GlobalNamespace::LevelFilteringNavigationController* self, System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::IBeatmapLevelPack*>* beatmapLevelPacks) {
    if (Il2CppString::IsNullOrEmpty(self->levelPackIdToBeSelectedAfterPresent)) {
        self->levelPackIdToBeSelectedAfterPresent = BetterSongList::Hooks::NavigationRestorePrepare::get_collection();
    }

    LevelFilteringNavigationController_ShowPacksInSecondChildController(self, beatmapLevelPacks);
}

// from HookSelectedCollection
MAKE_AUTO_HOOK_MATCH(LevelFilteringNavigationController_UpdateSecondChildControllerContent, &GlobalNamespace::LevelFilteringNavigationController::UpdateSecondChildControllerContent, void, GlobalNamespace::LevelFilteringNavigationController* self, GlobalNamespace::SelectLevelCategoryViewController::LevelCategory levelCategory) {
    BetterSongList::Hooks::HookSelectedCollection::LevelFilteringNavigationController_UpdateSecondChildControllerContent_Prefix(levelCategory);
    LevelFilteringNavigationController_UpdateSecondChildControllerContent(self, levelCategory);
}