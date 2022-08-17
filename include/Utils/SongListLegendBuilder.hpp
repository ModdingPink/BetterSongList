#pragma once

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include <string>
#include <unordered_map>
#include "ISorter.hpp"

namespace BetterSongList::SongListLegendBuilder {
    ISorterWithLegend::Legend BuildFor(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels, std::function<std::string(GlobalNamespace::IPreviewBeatmapLevel*)> builder, int entryLengthLimit = 6, int valueLimit = 28);
}