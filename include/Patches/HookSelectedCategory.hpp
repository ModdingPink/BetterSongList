#pragma once

#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"
namespace BetterSongList::Hooks {
    class HookSelectedCategory {
        public:
            static GlobalNamespace::SelectLevelCategoryViewController::LevelCategory get_lastSelectedCategory();
            /// @brief prio int min value
            static void SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell_Prefix(GlobalNamespace::SelectLevelCategoryViewController* self);
        private:
            static GlobalNamespace::SelectLevelCategoryViewController::LevelCategory lastSelectedCategory;
    };
}