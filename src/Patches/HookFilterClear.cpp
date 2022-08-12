#include "Patches/HookFilterClear.hpp"

#include "GlobalNamespace/LevelFilterParams.hpp"
#include "UI/FilterUI.hpp"

namespace BetterSongList::Hooks {
    void HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Prefix() {
        FilterUI::SetFilter("", false, false);
    }

    void HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Postfix(GlobalNamespace::LevelFilterParams* currentFilterParams) {
        if (currentFilterParams) currentFilterParams->searchText = "";
    }
}