#include "Sorters/SortMethods.hpp"
#include "logging.hpp"
#include "config.hpp"

#include "Sorters/Models/BasicSongDetailsSorterWithLegend.hpp"
#include "Sorters/Models/FolderDateSorter.hpp"
#include "Sorters/Models/FunctionSorter.hpp"

#include "sdc-wrapper/shared/BeatStarSong.hpp"

namespace BetterSongList {
    // TODO: define these
    static ComparableFunctionSorterWithLegend alphabeticalSongname(
        [](auto a, auto b) -> int {
            return static_cast<std::u16string_view>(a->get_songName()).compare(static_cast<std::u16string_view>(b->get_songName()));
        }, 
        [](auto song) -> std::string {
            std::string songName{song->get_songName()};
            return songName.size() > 0 ? songName.substr(0, 1) : "";
        }
    );

    static PrimitiveFunctionSorterWithLegend bpm(
        [](auto song){
            return song->get_beatsPerMinute();
        },
        [](auto song){
            return fmt::format("{}", std::round(song->get_beatsPerMinute()));
        }
    );

    static ComparableFunctionSorterWithLegend alphabeticalMapper(
        [](auto a, auto b) -> int {
            return static_cast<std::u16string_view>(a->get_levelAuthorName()).compare(static_cast<std::u16string_view>(b->get_levelAuthorName()));
        }, 
        [](auto song) -> std::string {
            std::string levelAuthor{song->get_levelAuthorName()};
            return levelAuthor.size() > 0 ? levelAuthor.substr(0, 1) : "";
        }
    );

    static FolderDateSorter downloadTime{};

    static std::optional<float> StarsProcessor(const SDC_wrapper::BeatStarSong* song) {
        auto diffs = song->GetDifficultyVector();
        float ret = 0;
        for (auto diff : diffs) {
            if (!diff->ranked) continue;
            if (diff->stars == 0) continue;
            if (ret == 0) {
                ret = diff->stars;
            }

            if (config.sortAsc) {
                if (ret < diff->stars) continue;
            } else if (ret > diff->stars) continue;

            ret = diff->stars;
        }

        if (ret == 0) return std::nullopt;
        return ret;
    }

    static BasicSongDetailsSorterWithLegend stars(
        StarsProcessor,
        [](auto song) -> std::string {
            auto y = StarsProcessor(song);
            if (y == std::nullopt) return "";

            return fmt::format("{:.1f}", y.value());
        }
    );

    static const float second = 1.0f / 60.0f;

    static PrimitiveFunctionSorterWithLegend songLength(
        [](auto song){ return song->get_songDuration(); },
        [](auto song) -> std::string {
            float duration = song->get_songDuration();
            if (duration < 60) return "<1";
            return fmt::format("{}", (int)std::round(duration * second));
        }
    );

    static BasicSongDetailsSorterWithLegend beatSaverDate(
        [](const SDC_wrapper::BeatStarSong* song) -> std::optional<float> { 
            return song->uploaded_unix_time;
        },
        [](const SDC_wrapper::BeatStarSong* song) -> std::string {
            auto uploaded = song->uploaded_unix_time;
            struct tm* tm = localtime(&uploaded);
            // since int division is floored, we can get an index for quarter by removing 1 and dividing by 3,
            // then we just add 1 back to get from 0-3 to 1-4
            auto q = ((tm->tm_mon - 1) / 3) + 1;

            // tm year is years since 1900, if we want years since 2000 we do -100
            // ex: 2022 is 122 since 1900, so 122 - 100 = 22 which is what we want to display
            auto y = tm->tm_year - 100;
            return fmt::format("Q{:1d} {:2d}", q, y);
        }
    );

    const std::map<std::string, ISorter*>& SortMethods::get_methods() {
        return methods;
    }

    bool SortMethods::Register(ITransformerPlugin* sorter) {
        auto name = sorter->get_name();
        if (name.size() > 20) {
            name = name.substr(0, 20);
            ERROR("Name of the Transformer can not be more than 20 characters!");
        }

        if (!config.allowPluginSortsAndFilters) return false;
        // TODO: check if this even works lol
        name = "🔌" + name;

        auto itr = methods.find(name);
        if (itr != methods.end()) {
            ERROR("sorter with name {} was already registerd!", name);
            return false;
        }

        methods.insert({ name, sorter });
        return true;
    }

    std::map<std::string, ISorter*> SortMethods::methods{
		{"Song Name", &alphabeticalSongname},
		{"Download Date", &downloadTime},
		{"Ranked Stars", &stars},
		{"Song Length", &songLength},
		{"BPM", &bpm},
		{"BeatSaver Date", &beatSaverDate},
		{"Default", nullptr}
    };
}