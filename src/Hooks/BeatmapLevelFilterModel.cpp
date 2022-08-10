#include "hooking.hpp"

#include "Patches/ImproveBasegameSearch.hpp"
#include "GlobalNamespace/BeatmapLevelFilterModel.hpp"

// from ImproveBasegameSearch
MAKE_AUTO_HOOK_ORIG_MATCH(BeatmapLevelFilterModel_LevelContainsText, &GlobalNamespace::BeatmapLevelFilterModel::LevelContainsText, bool, ::GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel, ArrayW<::StringW> searchTexts) {
    bool result = false;
    if (!BetterSongList::Hooks::ImproveBasegameSearch::BeatmapLevelFilterModel_LevelContainsText_Prefix(beatmapLevel, searchTexts, result)) {
        return result;
    }

    return BeatmapLevelFilterModel_LevelContainsText(beatmapLevel, searchTexts);
}