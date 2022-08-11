#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "UnityEngine/UI/Button.hpp"

namespace BetterSongList::Hooks {
    class SongDeleteButton {
        public:
            static void StandardLevelDetailView_RefreshContent_Postfix(GlobalNamespace::StandardLevelDetailView* self, UnityEngine::UI::Button* practiceButton, GlobalNamespace::IPreviewBeatmapLevel* level);
            static void UpdateState();
            static bool get_isWip();
            static GlobalNamespace::CustomPreviewBeatmapLevel* get_lastLevel();
        private:
            static SafePtr<GlobalNamespace::CustomPreviewBeatmapLevel> lastLevel;
            static SafePtrUnity<UnityEngine::UI::Button> deleteButton;
    };
}

DECLARE_CLASS_CODEGEN(BetterSongList, DeleteConfirmHandler, Il2CppObject,
    DECLARE_INSTANCE_FIELD(BSML::ModalView*, deleteModal);
    DECLARE_INSTANCE_METHOD(void, Confirm);
    DECLARE_INSTANCE_METHOD(void, ConfirmDelete);
    DECLARE_DEFAULT_CTOR();
    public:
        static DeleteConfirmHandler* get_instance();
    private:
        static SafePtr<DeleteConfirmHandler> instance;
)