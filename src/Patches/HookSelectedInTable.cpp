#include "Patches/HookSelectedInTable.hpp"
#include "config.hpp"
#include "logging.hpp"

namespace BetterSongList::Hooks {
    void HookSelectedInTable::LevelCollectionTableView_HandleDidSelectRowEvent_Postfix(GlobalNamespace::IPreviewBeatmapLevel* selectedPreviewBeatmapLevel) {
        config.set_lastSong(selectedPreviewBeatmapLevel ? static_cast<std::string>(selectedPreviewBeatmapLevel->get_levelID()) : "");
        WARNING("LevelCollectionTableView.HandleDidSelectRowEvent(): LastSong: {}", config.get_lastSong());
    }
}