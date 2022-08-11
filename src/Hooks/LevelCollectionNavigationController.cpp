#include "hooking.hpp"
#include "config.hpp"

#include "Patches/RestoreLevelSelection.hpp"
#include "Patches/UI/BottomUI.hpp"

#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"

// from UI/BottomUI
// from RestoreLevelSelection
MAKE_AUTO_HOOK_MATCH(LevelCollectionNavigationController_DidActivate, &GlobalNamespace::LevelCollectionNavigationController::DidActivate, void, GlobalNamespace::LevelCollectionNavigationController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::Hooks::RestoreLevelSelection::LevelCollectionNavigationController_DidActivate_Prefix(self, addedToHierarchy, self->beatmapLevelToBeSelectedAfterPresent);
    BetterSongList::Hooks::BottomUI::LevelCollectionNavigationController_DidActivate_Prefix(self, firstActivation);
    LevelCollectionNavigationController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    BetterSongList::Hooks::RestoreLevelSelection::LevelCollectionNavigationController_DidActivate_Postfix(addedToHierarchy, self->hideDetailViewController);
}

