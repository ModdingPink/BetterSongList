#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

namespace BetterSongList::Hooks {
    class HookSelectedInTable {
        public:
            static void LevelCollectionTableView_HandleDidSelectRowEvent_Postfix(GlobalNamespace::IPreviewBeatmapLevel* selectedPreviewBeatmapLevel);
    };
}