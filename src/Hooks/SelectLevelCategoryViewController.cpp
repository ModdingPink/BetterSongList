#include "hooking.hpp"

#include "Patches/HookSelectedCategory.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

// from HookSelectedCategory
MAKE_AUTO_HOOK_MATCH(SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell, &GlobalNamespace::SelectLevelCategoryViewController::LevelFilterCategoryIconSegmentedControlDidSelectCell, void, GlobalNamespace::SelectLevelCategoryViewController* self, HMUI::SegmentedControl* segmentedControl, int index) {
    BetterSongList::Hooks::HookSelectedCategory::SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell_Prefix(self);
    SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell(self, segmentedControl, index);
}