#include "Patches/UI/ExtraLevelParams.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "Utils/BeatmapPatternDetection.hpp"
#include "Utils/SongDetails.hpp"
#include "Utils/JumpDistanceCalculator.hpp"
#include "Utils/BeatmapUtils.hpp"
#include "Utils/PPUtils.hpp"

#include "bsml/shared/Helpers/utilities.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/HoverHint.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "GlobalNamespace/LocalizedHoverHint.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "GlobalNamespace/CustomDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData.hpp"

#include <algorithm>

namespace BetterSongList::Hooks {
    SafePtrUnity<GlobalNamespace::StandardLevelDetailView> ExtraLevelParams::lastInstance;
    SafePtrUnity<UnityEngine::GameObject> ExtraLevelParams::extraUI;
    SafePtr<Array<TMPro::TextMeshProUGUI*>> ExtraLevelParams::fields;
    SafePtrUnity<HMUI::HoverHintController> ExtraLevelParams::hoverHintController;
                
    void ExtraLevelParams::StandardLevelDetailView_RefreshContent_Postfix(GlobalNamespace::StandardLevelDetailView* self, GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::IDifficultyBeatmap* selectedDifficultyBeatmap, GlobalNamespace::LevelParamsPanel* levelParamsPanel) {
        if (!extraUI) {
            extraUI = UnityEngine::Object::Instantiate(levelParamsPanel->get_gameObject(), levelParamsPanel->get_transform()->get_parent());
            UnityEngine::Object::Destroy(extraUI->GetComponent<GlobalNamespace::LevelParamsPanel*>());
            
            bool isPostGagaUI = true;

            auto pos = levelParamsPanel->get_transform()->get_localPosition();
            pos.y += 3.5f;
            levelParamsPanel->get_transform()->set_localPosition(pos);
            pos = extraUI->get_transform()->get_localPosition();
            pos.y -= 1.0f;
            extraUI->get_transform()->set_localPosition(pos);

            fields.emplace(static_cast<Array<TMPro::TextMeshProUGUI*>*>(extraUI->GetComponentsInChildren<HMUI::CurvedTextMeshPro*>().convert()));
            GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(ProcessFields()));
        }

        lastInstance = self;

        auto obstaclesText = levelParamsPanel->obstaclesCountText;
        obstaclesText->set_fontStyle(TMPro::FontStyles::Italic);

        if (config.showWarningIfMapHasCrouchWallsBecauseMappersThinkSprinklingThemInRandomlyIsFun) {
            obstaclesText->set_richText(true);
            auto customdiffOpt = il2cpp_utils::try_cast<GlobalNamespace::CustomDifficultyBeatmap>(selectedDifficultyBeatmap);
            if (customdiffOpt.has_value()) {
                auto obstacles = customdiffOpt.value()->get_beatmapSaveData()->obstacles;
                if (BeatmapPatternDetection::CheckForCrouchWalls(obstacles)) {
                    obstaclesText->set_fontStyle(TMPro::FontStyles::Normal);
                    obstaclesText->set_text(fmt::format("<i>{}</i> <b><size=3.3><color=#FF0>⚠</color></size></b>", obstaclesText->get_text()));
                }
            } else {
                /* nothing */
            }

        }

        if (fields) {
            ArrayW<TMPro::TextMeshProUGUI*> fieldsW{fields.ptr()}; 
            if (!BetterSongList::SongDetails::get_isAvailable()) {
                fieldsW[0]->set_text("N/A");
                fieldsW[1]->set_text("N/A");
            } else if (!BetterSongList::SongDetails::get_songDetails().empty()) {
                auto parentSet = selectedDifficultyBeatmap->get_parentDifficultyBeatmapSet();
                auto characteristic = parentSet ? parentSet->get_beatmapCharacteristic() : nullptr;
                auto ch = characteristic ? SDC_wrapper::BeatStarCharacteristic::BeatmapCharacteristicToBeatStarCharacteristic(characteristic) : song_data_core::BeatStarCharacteristics::Unknown;
                
                if (ch != song_data_core::BeatStarCharacteristics::Standard) {
                    fieldsW[0]->set_text("-");
                    fieldsW[1]->set_text("-");
                } else {
                    auto hash = BeatmapUtils::GetHashOfPreview(level->i_IPreviewBeatmapLevel());
                    const SDC_wrapper::BeatStarSong* song = nullptr;
                    const SDC_wrapper::BeatStarSongDifficultyStats* diff = nullptr;
                    if (hash.empty() || /* no hash */
                        !(song = SDC_wrapper::BeatStarSong::GetSong(hash)) || /* song not found */
                        !(diff = song->GetDifficulty(ch, selectedDifficultyBeatmap->get_difficulty())) /* diff not found */
                    ) {
                        fieldsW[0]->set_text("?");
                        fieldsW[1]->set_text("?");
                    } else if (!diff->ranked) {
                        fieldsW[0]->set_text("-");
                        fieldsW[1]->set_text("-");
                    } else {
                        auto acc = 0.984f - (std::max(0.0f, (diff->stars - 1.5f) / (12.5f - 1.5f) / config.accuracyMultiplier) * .027f);
						auto pp = PPUtils::PPPercentage(acc) * diff->stars * 42.1f;

                        fieldsW[0]->set_text(fmt::format("{0} <size=2.5>({1:0.0f})</size>", (int)pp, acc));
						fieldsW[1]->set_text(fmt::format("{:1.1f}", diff->stars));
                    }
                }
            } else if (!BetterSongList::SongDetails::get_finishedInitAttempt()){
                GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(
                    custom_types::Helpers::CoroutineHelper::New(BetterSongList::SongDetails::TryGet([](){
                        if (!BetterSongList::SongDetails::get_songDetails().empty()) {
                            UpdateState();
                        }
                    }))
                );
            }

            auto njs = selectedDifficultyBeatmap->get_noteJumpMovementSpeed();
            if (njs == 0) {
                njs = GlobalNamespace::BeatmapDifficultyMethods::NoteJumpMovementSpeed(selectedDifficultyBeatmap->get_difficulty());
            }

            fieldsW[2]->set_text(fmt::format("{:1.1f}", njs));
            if (config.showMapJDInsteadOfOffset) { // map jump distance
                float offset = BetterSongList::JumpDistanceCalculator::GetJd(selectedDifficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_beatsPerMinute(), njs, selectedDifficultyBeatmap->get_noteJumpStartBeatOffset());
                fieldsW[3]->set_text(fmt::format("{:1.1f}", offset));
            } else { // offset
                fieldsW[3]->set_text(fmt::format("{:1.1f}", selectedDifficultyBeatmap->get_noteJumpStartBeatOffset()));
            }
        }
    }

    void ExtraLevelParams::UpdateState() {
        if (lastInstance && lastInstance->get_isActiveAndEnabled()) lastInstance->RefreshContent();
    }

    void ExtraLevelParams::ModifyValue(TMPro::TextMeshProUGUI* text, std::string_view hoverHint, std::string_view icon) {
        auto go = text->get_gameObject();
        BSML::Utilities::SetImage(text->get_transform()->get_parent()->Find("Icon")->get_gameObject()->GetComponent<HMUI::ImageView*>(), icon);
        UnityEngine::Object::DestroyImmediate(go->GetComponentInParent<GlobalNamespace::LocalizedHoverHint*>()->get_gameObject());
        auto hhint = go->GetComponentInParent<HMUI::HoverHint*>();

        if (!hhint) return;

        if (!hoverHintController) 
            hoverHintController = UnityEngine::Object::FindObjectOfType<HMUI::HoverHintController*>();
        hhint->hoverHintController = hoverHintController.ptr();
        hhint->set_text(hoverHint);
    }

    custom_types::Helpers::Coroutine ExtraLevelParams::ProcessFields() {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());
        ArrayW<TMPro::TextMeshProUGUI*> fieldsW{fields.ptr()};

        ModifyValue(fieldsW[0], "ScoreSaber PP Value", "#DifficultyIcon");
		ModifyValue(fieldsW[1], "ScoreSaber Star Rating", "#FavoritesIcon");
		ModifyValue(fieldsW[2], "NJS (Note Jump Speed)", "#FastNotesIcon");
		ModifyValue(fieldsW[3], "JD (Jump Distance, how close notes spawn)", "#MeasureIcon");

        fieldsW[0]->set_richText(true);
        fieldsW[0]->set_characterSpacing(-3.0f);
        fieldsW[3]->set_richText(true);
    }
}
