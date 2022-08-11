#pragma once

#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "custom-types/shared/coroutine.hpp"

namespace BetterSongList::Hooks {
    class BottomUI {
        public:
            static void LevelCollectionNavigationController_DidActivate_Prefix(GlobalNamespace::LevelCollectionNavigationController* self, bool firstActivation);
        
        private:
            static custom_types::Helpers::Coroutine FixPos(UnityEngine::Transform* t);
            static custom_types::Helpers::Coroutine InitDelayed(UnityEngine::Transform* t);
    };
}