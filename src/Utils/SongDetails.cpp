#include "Utils/SongDetails.hpp"
#include "logging.hpp"

#include "sdc-wrapper/shared/BeatStarSong.hpp"
#include <thread>

namespace BetterSongList::SongDetails {
    static std::vector<const SDC_wrapper::BeatStarSong*> songDetails;
    const std::vector<const SDC_wrapper::BeatStarSong*>& get_songDetails() {
        return songDetails;
    }

    bool get_isAvailable() {
        return CheckAvailable();
    }

    bool CheckAvailable() {
        return true;
    }

    static bool finishedInitAttempt = false;
    bool get_finishedInitAttempt() {
        return finishedInitAttempt;
    }
    
    static bool attemptedToInit = false;
    bool get_attemptedToInit() {
        return attemptedToInit;
    }

    std::string GetUnavailabilityReason() {
        if (!finishedInitAttempt) {
            return "Initialization is not complete";
        } else if (songDetails.size() == 0) {
            return "Could not find any songs";
        }
        return "";
    }

    void Init() {
        if (attemptedToInit) return;
        attemptedToInit = true;
        std::thread([](){
            songDetails = SDC_wrapper::BeatStarSong::GetAllSongs();
            finishedInitAttempt = true;
        }).detach();
    }
}