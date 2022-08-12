#include "Filters/Models/PlayedFilter.hpp"
#include "Utils/LocalScoresUtils.hpp"

namespace BetterSongList {
    PlayedFilter::PlayedFilter(bool unplayed) 
        : IFilter(), intendedPlayedState(!unplayed) {
        
    }

    bool PlayedFilter::get_isReady() const { 
        return LocalScoresUtils::get_hasScores(); 
    }

    void PlayedFilter::Prepare() {
        if (!get_isReady()) {
            LocalScoresUtils::Load();
        }
    }

    bool PlayedFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        if (!get_isReady())
            return true;

        return LocalScoresUtils::HasLocalScore(level) == intendedPlayedState;
    }
}