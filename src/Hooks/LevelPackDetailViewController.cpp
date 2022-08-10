#include "hooking.hpp"

#include "Patches/RestoreLevelSelection.hpp"
#include "GlobalNamespace/LevelPackDetailViewController.hpp"

MAKE_AUTO_HOOK_MATCH(LevelPackDetailViewController_RefreshAvailabilityAsync, &GlobalNamespace::LevelPackDetailViewController::RefreshAvailabilityAsync, void, GlobalNamespace::LevelPackDetailViewController* self) {
    if (BetterSongList::Hooks::RestoreLevelSelection::LevelPackDetailViewController_RefreshAvailabilityAsync_Prefix(self, self->pack))
        LevelPackDetailViewController_RefreshAvailabilityAsync(self);
}