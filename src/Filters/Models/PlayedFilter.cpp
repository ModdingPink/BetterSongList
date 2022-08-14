#include "Filters/Models/PlayedFilter.hpp"
#include "Utils/LocalScoresUtils.hpp"
#include "UnityEngine/Object.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

namespace BetterSongList {
    SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> PlayedFilter::loadingTask;

    PlayedFilter::PlayedFilter(bool unplayed) 
        : IFilter(), intendedPlayedState(!unplayed) {
        
    }

    bool PlayedFilter::get_isReady() const { 
        return LocalScoresUtils::get_hasScores(); 
    }

    System::Threading::Tasks::Task* PlayedFilter::Prepare() {
        if (!loadingTask) {
            loadingTask = System::Threading::Tasks::TaskCompletionSource_1<bool>::New_ctor();
            std::thread([](){
                while(!LocalScoresUtils::get_hasScores()) {
                    std::this_thread::yield();
                }

                loadingTask->TrySetResult(true);
                loadingTask.emplace(nullptr);
            }).detach();

            QuestUI::MainThreadScheduler::Schedule(LocalScoresUtils::Load);
        }
        return reinterpret_cast<System::Threading::Tasks::Task*>(loadingTask->get_Task()); 
    }

    bool PlayedFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        if (!get_isReady())
            return true;

        return LocalScoresUtils::HasLocalScore(level) == intendedPlayedState;
    }
}