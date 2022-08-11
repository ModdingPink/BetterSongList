#include "Utils/BeatmapPatternDetection.hpp"

namespace BetterSongList::BeatmapPatternDetection {
    bool CheckForCrouchWalls(ListWrapper<BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData*> obstacles) {
        if (!obstacles || obstacles.size()) return false;

        float wallExistence[2] = {0, 0};

        for (auto o : obstacles) {
            auto line = o->get_line();
            auto width = o->get_width();
			// Ignore 1 wide walls on left
            if (line == 3 || (line == 0 && width == 1)) continue;

            auto duration = o->get_duration();
			// Filter out fake walls, they dont drain energy
            if (duration < 0 || width <= 0) continue;

			// Detect >2 wide walls anywhere, or 2 wide wall in middle
            if (width > 2 || (width == 2 && line == 1)) {
                auto layer = o->get_layer();
                if (layer == 2 || (layer != 0 && (o->get_height() - layer >= 2)))
                    return true;
            }
            // Is the wall on the left or right half?
            bool isLeftHalf = line <= 1;

            // Check if the other half has an active wall, which would mean there is one on both halfs
			// I know this technically does not check if one of the halves is half-height, but whatever
            auto beat = o->get_beat();
            if (wallExistence[isLeftHalf ? 1 : 0] >= o->get_beat())
                return true;

			// Extend wall lengths by 120ms so that staggered crouchwalls that dont overlap are caught
			wallExistence[isLeftHalf ? 0 : 1] = std::max(wallExistence[isLeftHalf ? 0 : 1], beat + duration + 0.12f);
        }

        return false;
    }
}