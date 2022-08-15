#include "Patches/ImproveBasegameSearch.hpp"
#include "config.hpp"

namespace BetterSongList::Hooks {
    bool ImproveBasegameSearch::BeatmapLevelFilterModel_LevelContainsText_Prefix(GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel, ArrayW<StringW>& searchTexts, bool& result) {
        if (!config.get_modBasegameSearch()) return true;
        
        //searchTexts.Any(x => x.Length > 2 && beatmapLevel.levelAuthorName.IndexOf(x, 0, StringComparison.CurrentCultureIgnoreCase) != -1)
        for (auto s : searchTexts) {
            auto str = static_cast<std::u16string_view>(s);
            if (str.size() > 2) {
                auto authorName = static_cast<std::u16string_view>(beatmapLevel->get_levelAuthorName());

                // try to find the string in author name
                auto match = std::search(authorName.begin(), authorName.end(), str.begin(), str.end(), [](char16_t c1, char16_t c2) {
                    return std::toupper(c1) == std::toupper(c2);
                });

                // if match == first end, it was not found, otherwise it was found
                if (match != authorName.end()) {
                    result = true;
                    return false;
                }
            }
        }

        if (searchTexts.Length() > 1) {
            searchTexts = ArrayW<StringW>(il2cpp_array_size_t(1));
            searchTexts[0] = Il2CppString::Join(" ", searchTexts.convert());
        }

        return true;
    }
}