#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace BetterSongList::Hooks {
    class ImproveBasegameSearch {
        public:
            /// @brief prio int min value + 10
            static bool BeatmapLevelFilterModel_LevelContainsText_Prefix(GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel, ArrayW<StringW>& searchTexts, bool& result);
    };
}