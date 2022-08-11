#pragma once

#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/GameObject.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/HoverHintController.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LevelParamsPanel.hpp"

namespace BetterSongList::Hooks {
    class ExtraLevelParams {
        public:
            static void StandardLevelDetailView_RefreshContent_Postfix(
                    GlobalNamespace::StandardLevelDetailView* self, 
                    GlobalNamespace::IBeatmapLevel* level, 
                    GlobalNamespace::IDifficultyBeatmap* selectedDifficultyBeatmap,
                    GlobalNamespace::LevelParamsPanel* levelParamsPanel 
                );
            static void UpdateState();
            static void ModifyValue(TMPro::TextMeshProUGUI* text, std::string_view hoverHint, std::string_view icon);
        private:
            static custom_types::Helpers::Coroutine ProcessFields();
            static SafePtrUnity<GlobalNamespace::StandardLevelDetailView> lastInstance;
            static SafePtrUnity<UnityEngine::GameObject> extraUI;
            static SafePtr<Array<TMPro::TextMeshProUGUI*>> fields;
            static SafePtrUnity<HMUI::HoverHintController> hoverHintController;
    };
}