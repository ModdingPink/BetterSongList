#pragma once

#include <string>
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

struct Config {
    std::string lastSong = "";
    std::string lastPack = "";
    std::string lastSort = "";
    std::string lastFilter = "";
    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory lastCategory = GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None;
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