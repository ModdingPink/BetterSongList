#include "Filters/Models/PlayedFilter.hpp"
#include "Utils/LocalScoresUtils.hpp"
#include "UnityEngine/Object.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

namespace BetterSongList {
    PlayedFilter::PlayedFilter(bool unplayed) 
        : IFilter(), intendedPlayedState(!unplayed) {
        
    }

    bool PlayedFilter::get_isReady() const { 
        return LocalScoresUtils::get_hasScores(); 
    }

    std::future<void> PlayedFilter::Prepare() {
        QuestUI::MainThreadScheduler::Schedule(LocalScoresUtils::Load);
        return std::async(std::launch::deferred, [this](){
            while (!this->get_isReady()) std::this_thread::yield();
        });
    }

    bool PlayedFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        if (!get_isReady())
            return true;

        return LocalScoresUtils::HasLocalScore(level) == intendedPlayedState;
    }
}