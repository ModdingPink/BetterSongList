#include "Patches/RestoreTableScroll.hpp"
#include "config.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"
#include "System/Tuple_2.hpp"

namespace BetterSongList::Hooks {
    std::optional<int> RestoreTableScroll::scrollToIndex = std::nullopt;
    bool RestoreTableScroll::doResetScrollOnNext = false;
    bool RestoreTableScroll::gotoLastSelectedOnNextSetData = false;
    
    void RestoreTableScroll::ResetScroll() {
        scrollToIndex = 0;
        doResetScrollOnNext = true;
    }

    void RestoreTableScroll::GotoLastSelectedOnNextSetData() {
        gotoLastSelectedOnNextSetData = true;
	}
    void RestoreTableScroll::Prefix(GlobalNamespace::LevelCollectionTableView* self) {
        if (self->isInitialized && self->tableView != nullptr && self->previewBeatmapLevels != nullptr && !doResetScrollOnNext)
            scrollToIndex = self->tableView->GetVisibleCellsIdRange()->get_Item1();
    }
    
    void DoTheFunnySelect::PostFix(GlobalNamespace::LevelCollectionTableView* self) {
        auto tableView = self->tableView;
        auto previewBeatmapLevels = self->previewBeatmapLevels;
        auto showLevelPackHeader = self->showLevelPackHeader;

        bool specificMap = false;

        if (!RestoreTableScroll::scrollToIndex.has_value() || RestoreTableScroll::gotoLastSelectedOnNextSetData) {
            RestoreTableScroll::gotoLastSelectedOnNextSetData = false;

            int length = previewBeatmapLevels ? previewBeatmapLevels->i_IReadOnlyCollection_1_T()->get_Count() : 0;
            for (int i = 0; i < length; i++) {
                auto lvl = previewBeatmapLevels->get_Item(i);
                if (lvl->get_levelID() == config.lastSong) {
                    RestoreTableScroll::scrollToIndex = i + showLevelPackHeader;
                    specificMap = true;
                    break;
                }
            }
        }

        if (!RestoreTableScroll::scrollToIndex.has_value() || RestoreTableScroll::scrollToIndex.value() < 0) return;

        if (specificMap) {
            tableView->SelectCellWithIdx(RestoreTableScroll::scrollToIndex.value(), false);

            tableView->ScrollToCellWithIdx(RestoreTableScroll::scrollToIndex.value(), 
                specificMap ? HMUI::TableView::ScrollPositionType::Center : HMUI::TableView::ScrollPositionType::Beginning,
                specificMap);
        }
    }
}