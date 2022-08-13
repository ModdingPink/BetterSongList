#include "hooking.hpp"
#include "logging.hpp"

#include "Patches/RestoreTableScroll.hpp"
#include "Patches/HookLevelCollectionTableSet.hpp"
#include "Patches/UI/ScrollEnhancement.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"

// from RestoreTableScroll
// from UI/ScrollEnhancement
MAKE_AUTO_HOOK_MATCH(LevelCollectionTableView_Init_0, static_cast<void (GlobalNamespace::LevelCollectionTableView::*)()>(&GlobalNamespace::LevelCollectionTableView::Init), void, GlobalNamespace::LevelCollectionTableView* self) {
    BetterSongList::Hooks::RestoreTableScroll::Prefix(self);
    BetterSongList::Hooks::ScrollEnhancement::LevelCollectionTableView_Init_Prefix(self, self->isInitialized, self->tableView);
    LevelCollectionTableView_Init_0(self);
}

// from RestoreTableScroll
MAKE_AUTO_HOOK_MATCH(LevelCollectionTableView_Init_2, static_cast<void (GlobalNamespace::LevelCollectionTableView::*)(::StringW, ::UnityEngine::Sprite*)>(&GlobalNamespace::LevelCollectionTableView::Init), void, GlobalNamespace::LevelCollectionTableView* self, StringW headerText, UnityEngine::Sprite* headerSprite) {
    BetterSongList::Hooks::RestoreTableScroll::Prefix(self);
    LevelCollectionTableView_Init_2(self, headerText, headerSprite);
}

// from RestoreTableScroll
MAKE_AUTO_HOOK_MATCH(LevelCollectionTableView_SetData, &GlobalNamespace::LevelCollectionTableView::SetData, void, GlobalNamespace::LevelCollectionTableView* self, ::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::IPreviewBeatmapLevel*>* previewBeatmapLevels, ::System::Collections::Generic::HashSet_1<::StringW>* favoriteLevelIds, bool beatmapLevelsAreSorted) {
    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> arr{(void*)previewBeatmapLevels};
    BetterSongList::Hooks::HookLevelCollectionTableSet::Prefix(self, arr, favoriteLevelIds, beatmapLevelsAreSorted);
    LevelCollectionTableView_SetData(self, (decltype(previewBeatmapLevels))arr.convert(), favoriteLevelIds, beatmapLevelsAreSorted);
    BetterSongList::Hooks::DoTheFunnySelect::PostFix(self);
    BetterSongList::Hooks::HookLevelCollectionTableSet::PostFix(self, arr);
}