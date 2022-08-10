#include "hooking.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "Patches/NavigationRestorePrepare.hpp"

// from NavigationRestorePrepare
MAKE_AUTO_HOOK_MATCH(LevelSelectionFlowCoordinator_DidActivate, &GlobalNamespace::LevelSelectionFlowCoordinator::DidActivate, void, GlobalNamespace::LevelSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::Hooks::NavigationRestorePrepare::Prefix(self);
    LevelSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}