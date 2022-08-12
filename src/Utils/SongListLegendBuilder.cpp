#include "Utils/SongListLegendBuilder.hpp"

#include "GlobalNamespace/MenuLightsManager.hpp"
#include "sombrero/shared/linq_functional.hpp"
#include "bsml/shared/StringParseHelper.hpp"
#include <algorithm>

using namespace Sombrero::Linq::Functional;

struct LegendPair {
    LegendPair(const std::string& str, const int& num) : first(str), second(num) {}
    std::string first;
    int second;
};

struct ToUpperHelper : public std::string_view {
    ToUpperHelper(const std::string& str) : std::string_view(str) {}
    std::string toUpper() {
        std::string result{*this};
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
};

namespace BetterSongList::SongListLegendBuilder {
    std::map<std::string, int> BuildFor(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels, std::function<std::string(GlobalNamespace::IPreviewBeatmapLevel*)> builder, int entryLengthLimit, int valueLimit) {
        
        std::map<std::string, std::vector<LegendPair>> grouped;
        // start at -1 because 0 based index, and we have to pre increment due to the continue clause
        for (int i = -1; auto& level : levels) {
            i++;
            // make the str for the Select
            auto str = builder(level);
            /// the Where from the original method
            if (str.empty()) continue;

            // Grouping by key toUpper;
            auto KEY = ToUpperHelper(str).toUpper();
            auto itr = grouped.find(KEY);
            if (itr == grouped.end()) {
                grouped.emplace(KEY, std::vector<LegendPair>()).first->second.emplace_back(str, i);
            } else {
                itr->second.emplace_back(str, i);
            }
        }

        int grouped_size = grouped.size();
        int amt = std::min(grouped_size, valueLimit);
        std::map<std::string, int> result;
        if (amt > 1) {
            // get the keys at intervals with step size
            float step = ((float)(grouped_size - 1) / (float)(amt - 1));
            for (int i = 0; i < amt; i++) {
                auto bmi = (int)std::round(step * i);
                if (bmi > grouped_size - 1) bmi = grouped_size - 1;
                auto itr = std::next(grouped.begin(), bmi);
                auto transformedResult = itr->first;
                if (transformedResult.size() > entryLengthLimit)
                    transformedResult = transformedResult.substr(0, entryLengthLimit);
                result[transformedResult] = itr->second.front().second;
            }
        }
        return result;
    }
}