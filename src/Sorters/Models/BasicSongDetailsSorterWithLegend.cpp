#include "Sorters/Models/BasicSongDetailsSorterWithLegend.hpp"
#include "logging.hpp"

#include "Utils/SongDetails.hpp"
#include "Utils/BeatmapUtils.hpp"
#include "Utils/SongListLegendBuilder.hpp"

namespace BetterSongList {
    BasicSongDetailsSorterWithLegend::BasicSongDetailsSorterWithLegend(
        BasicSongDetailsSorterWithLegend::ValueGetterFunc sortFunc
    ) : 
        BasicSongDetailsSorterWithLegend(sortFunc, std::bind(&BasicSongDetailsSorterWithLegend::DefaultLegendGetter, this, std::placeholders::_1))
    {}

    BasicSongDetailsSorterWithLegend::BasicSongDetailsSorterWithLegend(
        BasicSongDetailsSorterWithLegend::ValueGetterFunc sortFunc, 
        BasicSongDetailsSorterWithLegend::LegendGetterFunc legendFunc
    ) : 
        ISorterWithLegend(),
        ISorterPrimitive(),
        IAvailabilityCheck(),
        sortValueGetter(sortFunc), 
        legendValueGetter(legendFunc) {}

    std::string BasicSongDetailsSorterWithLegend::DefaultLegendGetter(const SDC_wrapper::BeatStarSong* song) const {
        auto v = sortValueGetter(song);
        return v.has_value() ? fmt::format("{}", v.value()) : "";
    }

    bool BasicSongDetailsSorterWithLegend::get_isReady() const {
        return SongDetails::get_finishedInitAttempt();
    }

    void BasicSongDetailsSorterWithLegend::Prepare() {
        if (!get_isReady()) {
            // TODO: make this start right
            //SongDetails::TryGet();
        }
    }

    ISorterWithLegend::Legend BasicSongDetailsSorterWithLegend::BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const {
        if (SongDetails::get_songDetails().empty()) return {};

        return SongListLegendBuilder::BuildFor(levels, [legendValueGetter = legendValueGetter](GlobalNamespace::IPreviewBeatmapLevel* level) -> std::string {
            auto h = BeatmapUtils::GetHashOfPreview(level);
            if (h.empty()) return "N/A";

            auto song = SDC_wrapper::BeatStarSong::GetSong(h);
            if (!song) return "N/A";

            return legendValueGetter(song);
        });
    }

    std::optional<float> BasicSongDetailsSorterWithLegend::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const {
        if (SongDetails::get_songDetails().empty()) return std::nullopt;

        auto h = BeatmapUtils::GetHashOfPreview(level);
        if (h.empty()) return std::nullopt;

        auto song = SDC_wrapper::BeatStarSong::GetSong(h);
        if (!song) return std::nullopt;

        return sortValueGetter(song);
    }

    std::string BasicSongDetailsSorterWithLegend::GetUnavailableReason() const {
        return SongDetails::GetUnavailabilityReason();
    }
}