#pragma once

#include <string>

#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"

namespace BetterSongList::Hooks {
    class NavigationRestorePrepare {
        public:
            static std::string_view get_collection();
            static GlobalNamespace::SelectLevelCategoryViewController::LevelCategory get_category();
            static std::string_view get_level();

            static void Prefix(GlobalNamespace::LevelSelectionFlowCoordinator* self);
        private:
            static std::string levelPack;
            static GlobalNamespace::SelectLevelCategoryViewController::LevelCategory category;
            static std::string level;

    };
}