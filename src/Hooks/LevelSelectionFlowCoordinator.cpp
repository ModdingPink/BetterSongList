#include "hooking.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "Patches/RestoreLevelSelection.hpp"

// from RestoreLevelSelection
MAKE_AUTO_HOOK_MATCH(LevelSelectionFlowCoordinator_DidActivate, &GlobalNamespace::LevelSelectionFlowCoordinator::DidActivate, void, GlobalNamespace::LevelSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::Hooks::RestoreLevelSelection::LevelSelectionFlowCoordinator_DidActivate_Prefix(self->startState, firstActivation);
    LevelSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}