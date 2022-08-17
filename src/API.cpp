#include "API.hpp"
#include "conditional-dependencies/shared/main.hpp"

#include "Filters/FilterMethods.hpp"
#include "Sorters/SortMethods.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"

EXPOSE_API(RegisterPrimitiveSorter, bool, BetterSongList::ITransformerPlugin* plugin) {
    return BetterSongList::SortMethods::Register(plugin);
}

EXPOSE_API(RegisterCustomSorter, bool, BetterSongList::ITransformerPlugin* plugin) {
    return BetterSongList::SortMethods::Register(plugin);
}

EXPOSE_API(RegisterFilter, bool, BetterSongList::ITransformerPlugin* plugin) {
    return BetterSongList::FilterMethods::Register(plugin);
}

#pragma GCC diagnostic pop