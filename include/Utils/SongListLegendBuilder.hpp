#pragma once

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include <string>
#include <map>

namespace BetterSongList::SongListLegendBuilder {
    std::map<std::string, int> BuildFor(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels, std::function<std::string(GlobalNamespace::IPreviewBeatmapLevel*)> builder, int entryLengthLimit = 6, int valueLimit = 28);
}