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
    
    void RestoreTableScroll::ResetScroll() {
        scrollToIndex = 0;
        doResetScrollOnNext = true;

		INFO("RestoreTableScroll.ResetScroll()");
    }

    void RestoreTableScroll::LevelCollectionTableView_Init_Prefix(GlobalNamespace::LevelCollectionTableView* self) {
        if (self->isInitialized && self->tableView != nullptr && self->previewBeatmapLevels != nullptr && !doResetScrollOnNext)
            scrollToIndex = self->tableView->GetVisibleCellsIdRange()->get_Item1();

        doResetScrollOnNext = false;
		INFO("LevelCollectionTableView.Init():Prefix - scrollToIndex: {}", scrollToIndex);
    }

    void RestoreTableScroll::LevelCollectionTableView_SetData_PostFix(GlobalNamespace::LevelCollectionTableView* self) {
		INFO("DoTheFunnySelect -> LevelCollectionTableView.SetData():Postfix scrollToIndex: {}", RestoreTableScroll::scrollToIndex);
        auto tableView = self->tableView;
        auto previewBeatmapLevels = self->previewBeatmapLevels;
        auto showLevelPackHeader = self->showLevelPackHeader;

        if (!RestoreTableScroll::scrollToIndex.has_value() || RestoreTableScroll::scrollToIndex.value() < 0) return;

		INFO("-> Scrolling to {}", RestoreTableScroll::scrollToIndex);

        tableView->ScrollToCellWithIdx(
            RestoreTableScroll::scrollToIndex.value(),
            HMUI::TableView::ScrollPositionType::Beginning,
            false
        );
        INFO("Scroll done");
    }
}