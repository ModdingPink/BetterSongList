#include "Filters/FilterMethods.hpp"
#include "logging.hpp"
#include "config.hpp"

#include "Filters/Models/BasicSongDetailsFilter.hpp"
#include "Filters/Models/FunctionFilter.hpp"
#include "Filters/Models/PlayedFilter.hpp"
#include "Filters/Models/RequirementsFilter.hpp"

namespace BetterSongList {
    static BasicSongDetailsFilter ranked([](const SDC_wrapper::BeatStarSong* x){ 
        auto diffs = x->GetDifficultyVector();
        for (auto diff : diffs) if (diff->ranked) return true;
        return false;
    });
    
    static BasicSongDetailsFilter unranked([](const SDC_wrapper::BeatStarSong* x){ 
        auto diffs = x->GetDifficultyVector();
        for (auto diff : diffs) if (diff->ranked) return false;
        return true;
    });

    static BasicSongDetailsFilter qualified([](const SDC_wrapper::BeatStarSong* x) {
        // FIXME: this filter is not gonna be done right now
        // mostly because the data is not available in SDC and making it available is a not now thing
        return false;
    });

    static PlayedFilter unplayed(true);
    static PlayedFilter played{};
    static RequirementsFilter requirements(false);
    static RequirementsFilter noRequirements(true);

    const std::map<std::string, IFilter*>& FilterMethods::get_methods() {
        return methods;
    }

    bool FilterMethods::Register(ITransformerPlugin* filter) {
        auto name = filter->get_name();
        if (name.size() > 20) {
            name = name.substr(0, 20);
            ERROR("Name of the Transformer can not be more than 20 characters!");
        }

        if (!config.get_allowPluginSortsAndFilters()) return false;
        name = "P " + name;

        auto itr = methods.find(name);
        if (itr != methods.end()) {
            ERROR("filter with name {} was already registerd!", name);
            return false;
        }

        methods.insert({ name, reinterpret_cast<IFilter*>(filter) });
        return true;
    }

    std::map<std::string, IFilter*> FilterMethods::methods{
        {"Ranked", &ranked},
		{"Unplayed", &unplayed},
		{"Played", &played},
		{"Requirements", &requirements},
		{"No Requirements", &noRequirements},
		{"Unranked", &unranked},
		{"All", nullptr}
    };
}