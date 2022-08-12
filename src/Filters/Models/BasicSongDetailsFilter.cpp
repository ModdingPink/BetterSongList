#include "Filters/Models/BasicSongDetailsFilter.hpp"

#include "Utils/SongDetails.hpp"
#include "Utils/BeatmapUtils.hpp"

namespace BetterSongList {
    BasicSongDetailsFilter::BasicSongDetailsFilter(const std::function<bool(const SDC_wrapper::BeatStarSong*)>& func) 
        : IFilter(), IAvailabilityCheck(), filterValueTransformer(func) {

    }

    bool BasicSongDetailsFilter::get_isReady() const { 
        return SongDetails::get_finishedInitAttempt(); 
    }

    void BasicSongDetailsFilter::Prepare() {

    }
    
    bool BasicSongDetailsFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        if (SongDetails::get_songDetails().empty()) {
            return false;
        }

        auto h = BeatmapUtils::GetHashOfPreview(level);
        if (h.empty()) return false;

        auto song = SDC_wrapper::BeatStarSong::GetSong(h);
        if (!song) return false;

        return filterValueTransformer(song);
    }

    std::string BasicSongDetailsFilter::GetUnavailableReason() const { 
        return SongDetails::GetUnavailabilityReason(); 
    }
}