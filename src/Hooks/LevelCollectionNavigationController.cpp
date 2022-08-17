#include "hooking.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "Patches/UI/BottomUI.hpp"
#include "UI/FilterUI.hpp"

#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/LevelPackDetailViewController.hpp"

#include "bsml/shared/Helpers/delegates.hpp"

// from UI/BottomUI
MAKE_AUTO_HOOK_MATCH(LevelCollectionNavigationController_DidActivate, &GlobalNamespace::LevelCollectionNavigationController::DidActivate, void, GlobalNamespace::LevelCollectionNavigationController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::Hooks::BottomUI::LevelCollectionNavigationController_DidActivate_Prefix(self, firstActivation);
    LevelCollectionNavigationController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}