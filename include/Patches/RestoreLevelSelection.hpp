#pragma once

#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"

namespace BetterSongList::Hooks {
    class RestoreLevelSelection {
        public:
            /// @brief no prio
            static void LevelSelectionFlowCoordinator_DidActivate_Prefix(GlobalNamespace::LevelSelectionFlowCoordinator::State*& startState, bool firstActivation);
            /// @brief prio int minvalue
            static void LevelFilteringNavigationController_ShowPacksInSecondChildController_Prefix(StringW& levelPackIdToBeSelectedAfterPresent);
        private:
            static void LoadPackFromCollectionName();
            static std::string restoredPackId;
            static SafePtr<GlobalNamespace::IBeatmapLevelPack> restoredPack;
    };
}