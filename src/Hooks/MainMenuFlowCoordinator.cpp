#include "hooking.hpp"

#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "UI/FilterUI.hpp"

MAKE_AUTO_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    BetterSongList::FilterUI::Init();
    MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}