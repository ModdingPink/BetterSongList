#include "hooking.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "Patches/UI/SongDeleteButton.hpp"
#include "Patches/UI/ExtraLevelParams.hpp"

// from UI/SongDeleteButton
// from UI/ExtraLevelParams
MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);
    //BetterSongList::Hooks::SongDeleteButton::StandardLevelDetailView_RefreshContent_Postfix(self, self->get_practiceButton(), self->level->i_IPreviewBeatmapLevel());
    BetterSongList::Hooks::ExtraLevelParams::StandardLevelDetailView_RefreshContent_Postfix(self, self->level, self->get_selectedDifficultyBeatmap(), self->levelParamsPanel);
}