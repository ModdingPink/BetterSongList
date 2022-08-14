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
    LevelCategory lastCategory = LevelCategory::All;
    bool reselectLastSong = true;
    bool enableAlphabetScrollBar = true;
    bool clearFiltersOnPlaylistSelect = true;
    bool modBasegameSearch = true;
    bool autoFilterUnowned = true;
    bool extendSongScrollbar = true;
    bool allowWipDelete = false;
    bool showWarningIfMapHasCrouchWallsBecauseMappersThinkSprinklingThemInRandomlyIsFun = true;
    bool showMapJDInsteadOfOffset = true;
    float accuracyMultiplier = 1.0f;
    bool allowPluginSortsAndFilters = true;
    bool sortAsc;
    std::string settingsSeenInVersion = "";
};

extern Config config;

void SaveConfig();
bool LoadConfig();