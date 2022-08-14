#pragma once

#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"

namespace BetterSongList::Hooks {
    class RestoreLevelSelection {
        public:
            static void LevelSelectionFlowCoordinator_DidActivate_Prefix(GlobalNamespace::LevelSelectionFlowCoordinator::State*& startState);
            static void LevelFilteringNavigationController_ShowPacksInSecondChildController_Prefix(StringW& levelPackIdToBeSelectedAfterPresent);
        private:
            static void LoadPackFromCollectionName();
            static std::string restoredPackId;
            static SafePtr<GlobalNamespace::BeatmapLevelPack> restoredPack;
    };
}