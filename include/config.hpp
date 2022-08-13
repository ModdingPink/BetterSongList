#pragma once

#include <string>
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"
// honestly fuck typing this out completely every time
using LevelCategory = GlobalNamespace::SelectLevelCategoryViewController::LevelCategory;

struct Config {
    std::string lastSong = "";
    std::string lastPack = "";
    std::string lastSort = "";
    std::string lastFilter = "";
    LevelCategory lastCategory = LevelCategory::None;
    bool reselectLastSong;
    bool sortAscending;
    bool enableAlphabetScrollBar;
    bool clearFiltersOnPlaylistSelect;
    bool modBasegameSearch;
    bool autoFilterUnowned;
    bool extendSongScrollbar;
    bool allowWipDelete;
    bool showWarningIfMapHasCrouchWallsBecauseMappersThinkSprinklingThemInRandomlyIsFun;
    bool showMapJDInsteadOfOffset;
    float accuracyMultiplier;
    bool allowPluginSortsAndFilters;
    bool sortAsc;
    std::string settingsSeenInVersion = "";
};

extern Config config;

void SaveConfig();
bool LoadConfig();