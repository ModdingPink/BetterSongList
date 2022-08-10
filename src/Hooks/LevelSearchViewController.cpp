#include "hooking.hpp"

#include "Patches/HookFilterClear.hpp"
#include "GlobalNamespace/LevelSearchViewController.hpp"

// from HookFilterClear
MAKE_AUTO_HOOK_MATCH(LevelSearchViewController_ResetCurrentFilterParams, &GlobalNamespace::LevelSearchViewController::ResetCurrentFilterParams, void, GlobalNamespace::LevelSearchViewController* self) {
    BetterSongList::Hooks::HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Prefix();
    LevelSearchViewController_ResetCurrentFilterParams(self);
    BetterSongList::Hooks::HookFilterClear::LevelSearchViewController_ResetCurrentFilterParams_Postfix(self->currentFilterParams);
}

// from PreventLevelSearchViewControllerDoubleInit
MAKE_AUTO_HOOK_MATCH(LevelSearchViewController_ResetFilterParams, &GlobalNamespace::LevelSearchViewController::ResetFilterParams, void, GlobalNamespace::LevelSearchViewController* self, bool onlyFavorites) {
    self->onlyFavorites = onlyFavorites;
    if (self->get_isActiveAndEnabled())
        LevelSearchViewController_ResetFilterParams(self, onlyFavorites);
}
