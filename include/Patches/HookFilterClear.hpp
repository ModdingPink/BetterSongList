#pragma once

#include "GlobalNamespace/LevelSearchViewController.hpp"
namespace BetterSongList::Hooks {
    class HookFilterClear {
        public:
            static void LevelSearchViewController_ResetCurrentFilterParams_Prefix();
    };
}