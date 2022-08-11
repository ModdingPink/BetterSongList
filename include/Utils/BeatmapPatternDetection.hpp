#pragma once

#include "BeatmapSaveDataVersion3/BeatmapSaveData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_ObstacleData.hpp"

namespace BetterSongList::BeatmapPatternDetection {
    bool CheckForCrouchWalls(ListWrapper<BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData*> obstacles);
}