#include "hooking.hpp"

#include "Patches/HookSelectedCategory.hpp"
#include "Patches/RestoreLevelSelection.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

// from HookSelectedCategory
MAKE_AUTO_HOOK_MATCH(SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell, &GlobalNamespace::SelectLevelCategoryViewController::LevelFilterCategoryIconSegmentedControlDidSelectCell, void, GlobalNamespace::SelectLevelCategoryViewController* self, HMUI::SegmentedControl* segmentedControl, int index) {
    BetterSongList::Hooks::HookSelectedCategory::SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell_Prefix(self);
    SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell(self, segmentedControl, index);
}

// from RestoreLevelSelection
MAKE_AUTO_HOOK_MATCH(SelectLevelCategoryViewController_Setup, &GlobalNamespace::SelectLevelCategoryViewController::Setup, void, GlobalNamespace::SelectLevelCategoryViewController* self, GlobalNamespace::SelectLevelCategoryViewController::LevelCategory selectedCategory, ArrayW<GlobalNamespace::SelectLevelCategoryViewController::LevelCategory> enabledLevelCategories) {
    BetterSongList::Hooks::RestoreLevelSelection::SelectLevelCategoryViewController_Setup_Prefix(selectedCategory);
    SelectLevelCategoryViewController_Setup(self, selectedCategory, enabledLevelCategories);
}