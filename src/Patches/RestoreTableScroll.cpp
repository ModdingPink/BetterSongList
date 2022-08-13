#include "Patches/RestoreTableScroll.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"
#include "System/Tuple_2.hpp"

template <> struct fmt::formatter<std::optional<int>> : formatter<string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(std::optional<int> o, FormatContext& ctx) {
        if (o.has_value()) {
            return formatter<string_view>::format(std::to_string(o.value()), ctx);
        } else {
            return formatter<string_view>::format("nullopt", ctx);
        }
    }
};


namespace BetterSongList::Hooks {
    std::optional<int> RestoreTableScroll::scrollToIndex = std::nullopt;
    bool RestoreTableScroll::doResetScrollOnNext = false;
    bool RestoreTableScroll::gotoLastSelectedOnNextSetData = false;
    
    void RestoreTableScroll::ResetScroll() {
        scrollToIndex = 0;
        doResetScrollOnNext = true;

		WARNING("RestoreTableScroll.ResetScroll()");
    }

    void RestoreTableScroll::GotoLastSelectedOnNextSetData() {
        gotoLastSelectedOnNextSetData = true;
	}
    void RestoreTableScroll::Prefix(GlobalNamespace::LevelCollectionTableView* self) {
        if (self->isInitialized && self->tableView != nullptr && self->previewBeatmapLevels != nullptr && !doResetScrollOnNext)
            scrollToIndex = self->tableView->GetVisibleCellsIdRange()->get_Item1();

        doResetScrollOnNext = false;
		WARNING("LevelCollectionTableView.Init():Prefix - scrollToIndex: {}", scrollToIndex);
    }
    
    void DoTheFunnySelect::PostFix(GlobalNamespace::LevelCollectionTableView* self) {
		WARNING("DoTheFunnySelect -> LevelCollectionTableView.SetData():Postfix scrollToIndex: {}", RestoreTableScroll::scrollToIndex);
        auto tableView = self->tableView;
        auto previewBeatmapLevels = self->previewBeatmapLevels;
        auto showLevelPackHeader = self->showLevelPackHeader;

        bool specificMap = false;

        if (!RestoreTableScroll::scrollToIndex.has_value() || RestoreTableScroll::gotoLastSelectedOnNextSetData) {
            RestoreTableScroll::gotoLastSelectedOnNextSetData = false;

            INFO("Trying to find the correct index");
            if (!config.lastSong.empty()) {
                int length = previewBeatmapLevels ? previewBeatmapLevels->i_IReadOnlyCollection_1_T()->get_Count() : 0;
                for (int i = 0; i < length; i++) {
                    auto lvl = previewBeatmapLevels->get_Item(i);
                    if (lvl->get_levelID() == config.lastSong) {
                        INFO("Got the level!");
                        RestoreTableScroll::scrollToIndex = i + showLevelPackHeader;
                        specificMap = true;
                        break;
                    }
                }
            }
        }

        if (!RestoreTableScroll::scrollToIndex.has_value() || RestoreTableScroll::scrollToIndex.value() < 0) return;

		WARNING("-> Scrolling to {} (Specific map: {})", RestoreTableScroll::scrollToIndex, specificMap);

        if (specificMap) {
            tableView->SelectCellWithIdx(RestoreTableScroll::scrollToIndex.value(), false);

            tableView->ScrollToCellWithIdx(RestoreTableScroll::scrollToIndex.value(), 
                specificMap ? HMUI::TableView::ScrollPositionType::Center : HMUI::TableView::ScrollPositionType::Beginning,
                specificMap);
        }
    }
}