#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "Zenject/DiContainer.hpp"
#include "System/Action_1.hpp"

#include "Patches/UI/ScrollEnhancement.hpp"

MAKE_AUTO_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &GlobalNamespace::MenuTransitionsHelper::RestartGame, void, GlobalNamespace::MenuTransitionsHelper* self, ::System::Action_1<::Zenject::DiContainer*>* finishCallback) {
    BetterSongList::Hooks::ScrollEnhancement::GameRestart();
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}