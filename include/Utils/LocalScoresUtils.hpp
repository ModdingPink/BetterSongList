#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
namespace BetterSongList::LocalScoresUtils {
    bool get_hasScores();
    bool HasLocalScore(GlobalNamespace::IPreviewBeatmapLevel* level);
    void Load();
}