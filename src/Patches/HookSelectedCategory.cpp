#include "Patches/HookSelectedCategory.hpp"
#include "Patches/RestoreTableScroll.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "UI/FilterUI.hpp"

namespace BetterSongList::Hooks {
    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory HookSelectedCategory::lastSelectedCategory = GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None;
    
    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory HookSelectedCategory::get_lastSelectedCategory() {
        return lastSelectedCategory;
    }
    
    void HookSelectedCategory::SelectLevelCategoryViewController_LevelFilterCategoryIconSegmentedControlDidSelectCell_Prefix(GlobalNamespace::SelectLevelCategoryViewController* self) {
		DEBUG("SelectLevelCategoryViewController.LevelFilterCategoryIconSegmentedControlDidSelectCell():Prefix");
        if (lastSelectedCategory == self->get_selectedLevelCategory()) {
            return;
        }

        lastSelectedCategory = self->get_selectedLevelCategory();
        config.set_lastCategory(lastSelectedCategory);

        RestoreTableScroll::ResetScroll();

		INFO("SelectLevelCategoryViewController.LevelFilterCategoryIconSegmentedControlDidSelectCell():Prefix => ResetScroll()");
        auto instance = FilterUI::get_instance();
        instance->UpdateTransformerOptionsAndDropdowns();
    }
}