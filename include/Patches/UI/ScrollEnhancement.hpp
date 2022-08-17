#pragma once

#include "custom-types/shared/coroutine.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "UnityEngine/GameObject.hpp"
#include "HMUI/TableView.hpp"

namespace BetterSongList::Hooks {
    class ScrollEnhancement {
        public:
            /// @brief no prio
            static void LevelCollectionTableView_Init_Prefix(GlobalNamespace::LevelCollectionTableView* self, bool isInitialized, HMUI::TableView* tableView);
            static void UpdateState();
        private:
            static UnityEngine::Transform* BuildButton(UnityEngine::Transform* baseButton, StringW Icon, float vOffs, float rotation, std::function<void()> cb);
            static custom_types::Helpers::Coroutine SetupExtraScrollButtons(HMUI::TableView* table, UnityEngine::Transform* a);
            static std::array<SafePtrUnity<UnityEngine::GameObject>, 4> buttons;
            
    };
}