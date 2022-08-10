#include "hooking.hpp"
#include "logging.hpp"

#include "Patches/RestoreTableScroll.hpp"
#include "Patches/RestoreLevelSelection.hpp"
#include "Patches/HookLevelCollectionTableSet.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"

// from RestoreLevelSelection
MAKE_AUTO_HOOK_ORIG_MATCH(LevelCollectionTableView_SelectLevel, &GlobalNamespace::LevelCollectionTableView::SelectLevel, void, GlobalNamespace::LevelCollectionTableView* self, GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel) {
    if (BetterSongList::Hooks::RestoreLevelSelection::LevelCollectionTableView_SelectLevel_Prefix(self->previewBeatmapLevels))
        LevelCollectionTableView_SelectLevel(self, beatmapLevel);
}

// from RestoreTableScroll
MAKE_AUTO_HOOK_MATCH(LevelCollectionTableView_Init_0, static_cast<void (GlobalNamespace::LevelCollectionTableView::*)()>(&GlobalNamespace::LevelCollectionTableView::Init), void, GlobalNamespace::LevelCollectionTableView* self) {
    BetterSongList::Hooks::RestoreTableScroll::Prefix(self);
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