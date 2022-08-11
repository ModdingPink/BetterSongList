#include "Utils/BeatmapUtils.hpp"


#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

namespace BetterSongList::BeatmapUtils {
    std::string GetHashOfPreview(GlobalNamespace::IPreviewBeatmapLevel* level) {
        if (!level) return "";

        auto levelId = static_cast<std::string>(level->get_levelID());
        if (levelId.size() < 53) return "";
        if (levelId[12] != '_') return "";
        return levelId.substr(13, 40);
    }
    
    int GetCharacteristicFromDifficulty(GlobalNamespace::IDifficultyBeatmap* diff) {
        auto parentSet = diff->get_parentDifficultyBeatmapSet();
        auto d = parentSet ? parentSet->get_beatmapCharacteristic()->get_sortingOrder() : 0;
        if (d == 0 || d > 4) return 0;

        if (d == 3)
            d = 4;
        else if (d == 4)
            d = 3;

        return d + 1;
    }
}