#include "hooking.hpp"
#include "Patches/HookSelectedCollection.hpp"

#include "GlobalNamespace/LevelFilteringNavigationController.hpp"

// from HookSelectedCollection
MAKE_AUTO_HOOK_MATCH(LevelFilteringNavigationController_UpdateSecondChildControllerContent, &GlobalNamespace::LevelFilteringNavigationController::UpdateSecondChildControllerContent, void, GlobalNamespace::LevelFilteringNavigationController* self, GlobalNamespace::SelectLevelCategoryViewController::LevelCategory levelCategory) {
    BetterSongList::Hooks::HookSelectedCollection::LevelFilteringNavigationController_UpdateSecondChildControllerContent_Prefix(levelCategory);
    LevelFilteringNavigationController_UpdateSecondChildControllerContent(self, levelCategory);
}