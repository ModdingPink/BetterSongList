#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include <string>

namespace BetterSongList::BeatmapUtils {
    std::string GetHashOfPreview(GlobalNamespace::IPreviewBeatmapLevel* level);
    int GetCharacteristicFromDifficulty(GlobalNamespace::IDifficultyBeatmap* diff);
}