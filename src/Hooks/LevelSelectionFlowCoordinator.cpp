#include "hooking.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "Patches/RestoreLevelSelection.hpp"

// from RestoreLevelSelection
MAKE_AUTO_HOOK_MATCH(LevelSelectionFlowCoordinator_DidActivate, &GlobalNamespace::LevelSelectionFlowCoordinator::DidActivate, void, GlobalNamespace::LevelSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::Hooks::RestoreLevelSelection::LevelSelectionFlowCoordinator_DidActivate_Prefix(self->startState);
    if (self->startState) {
        auto startState = self->startState;
        if (startState->levelCategory.get_HasValue()) {
            INFO("levelCategory: {}", self->startState->levelCategory.get_Value().value);
        }

        if (startState->beatmapLevelPack) {
            auto levelPack = startState->beatmapLevelPack;
            auto packId = levelPack->get_packID();
            INFO("levelPackId: {}", packId ? packId : "NULL");
            auto packName = levelPack->get_packName();
            INFO("levelPackName: {}", packName ? packName : "NULL");
            auto shortPackName = levelPack->get_shortPackName();
            INFO("levelPackShortName: {}", shortPackName ? shortPackName : "NULL");
        }
    }
    LevelSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}