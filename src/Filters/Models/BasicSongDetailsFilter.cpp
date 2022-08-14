#include "Filters/Models/BasicSongDetailsFilter.hpp"

#include "Utils/SongDetails.hpp"
#include "Utils/BeatmapUtils.hpp"

namespace BetterSongList {
    SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> BasicSongDetailsFilter::loadingTask;

    BasicSongDetailsFilter::BasicSongDetailsFilter(const std::function<bool(const SDC_wrapper::BeatStarSong*)>& func) 
        : IFilter(), IAvailabilityCheck(), filterValueTransformer(func) {

    }

    bool BasicSongDetailsFilter::get_isReady() const { 
        return SongDetails::get_finishedInitAttempt(); 
    }

    System::Threading::Tasks::Task* BasicSongDetailsFilter::Prepare() {
        if (!loadingTask) {
            loadingTask = System::Threading::Tasks::TaskCompletionSource_1<bool>::New_ctor();
            std::thread([](){
                while(!SongDetails::get_finishedInitAttempt()) {
                    std::this_thread::yield();
                }

                loadingTask->TrySetResult(true);
                loadingTask.emplace(nullptr);
            }).detach();
            return reinterpret_cast<System::Threading::Tasks::Task*>(loadingTask->get_Task());
        }

        return System::Threading::Tasks::Task::get_CompletedTask();
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