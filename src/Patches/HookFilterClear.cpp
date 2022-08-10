#include "Patches/HookFilterClear.hpp"

#include "GlobalNamespace/LevelFilterParams.hpp"

namespace BetterSongList::Hooks {
    void HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Prefix() {
        //TODO: filterui stuff
        //FilterUI.SetFilter(null, false, false);
    }

    void HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Postfix(GlobalNamespace::LevelFilterParams* currentFilterParams) {
        if (currentFilterParams) currentFilterParams->searchText = "";
    }
}