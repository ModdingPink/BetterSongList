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

std::array<std::string, 5> diffToString {
    "Easy",
	"Normal",
	"Hard",
	"Expert",
	"ExpertPlus",
};

std::string_view BeatmapDifficultyToString(int value) {
    return diffToString.at(value);
}

namespace BetterSongList::Hooks {
    SafePtrUnity<GlobalNamespace::StandardLevelDetailView> ExtraLevelParams::lastInstance;
    SafePtrUnity<UnityEngine::GameObject> ExtraLevelParams::extraUI;
    SafePtr<Array<TMPro::TextMeshProUGUI*>> ExtraLevelParams::fields;
    SafePtrUnity<HMUI::HoverHintController> ExtraLevelParams::hoverHintController;
                
    void ExtraLevelParams::StandardLevelDetailView_RefreshContent_Postfix(GlobalNamespace::StandardLevelDetailView* self, GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::IDifficultyBeatmap* selectedDifficultyBeatmap, GlobalNamespace::LevelParamsPanel* levelParamsPanel) {
        if (!extraUI) {
            extraUI = UnityEngine::Object::Instantiate(levelParamsPanel->get_gameObject(), levelParamsPanel->get_transform()->get_parent());
            UnityEngine::Object::Destroy(extraUI->GetComponent<GlobalNamespace::LevelParamsPanel*>());
            
            auto pos = levelParamsPanel->get_transform()->get_localPosition();
            pos.y += 3.5f;
            levelParamsPanel->get_transform()->set_localPosition(pos);
            pos = extraUI->get_transform()->get_localPosition();
            pos.y -= 1.0f;
            extraUI->get_transform()->set_localPosition(pos);

            fields.emplace(static_cast<Array<TMPro::TextMeshProUGUI*>*>(extraUI->GetComponentsInChildren<HMUI::CurvedTextMeshPro*>(true).convert()));
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
                INFO("No song details available");
                fieldsW[0]->set_text("N/A");
                fieldsW[1]->set_text("N/A");
            } else if (!BetterSongList::SongDetails::get_songDetails().empty()) {
                INFO("details available, not empty");
                auto parentSet = selectedDifficultyBeatmap->get_parentDifficultyBeatmapSet();
                auto characteristic = parentSet ? parentSet->get_beatmapCharacteristic() : nullptr;
                auto ch = characteristic ? SDC_wrapper::BeatStarCharacteristic::BeatmapCharacteristicToBeatStarCharacteristic(characteristic) : song_data_core::BeatStarCharacteristics::Unknown;
                
                if (ch != song_data_core::BeatStarCharacteristics::Standard) {
                    INFO("Characteristic was not standard");
                    fieldsW[0]->set_text("-");
                    fieldsW[1]->set_text("-");
                } else {
                    INFO("Characteristic was standard");
                    auto hash = BeatmapUtils::GetHashOfPreview(level->i_IPreviewBeatmapLevel());
                    INFO("Got hash: {}", hash);
                    const SDC_wrapper::BeatStarSong* song = nullptr;
                    const SDC_wrapper::BeatStarSongDifficultyStats* diff = nullptr;
                    if (hash.empty() || /* no hash */
                        !(song = SDC_wrapper::BeatStarSong::GetSong(hash)) || /* song not found */
                        !(diff = song->GetDifficulty(ch, BeatmapDifficultyToString(selectedDifficultyBeatmap->get_difficulty()))) /* diff not found */
                    ) {
                        INFO("either hash was empty, the song was not found, or the diff was not found");
                        fieldsW[0]->set_text("?");
                        fieldsW[1]->set_text("?");
                    } else if (!diff->ranked) {
                        INFO("the diff was not ranked");
                        fieldsW[0]->set_text("-");
                        fieldsW[1]->set_text("-");
                    } else {
                        INFO("diff was found and ranked, now to show those values");
                        auto acc = 0.984f - (std::max(0.0f, (diff->stars - 1.5f) / (12.5f - 1.5f) / config.accuracyMultiplier) * .027f);
						auto pp = PPUtils::PPPercentage(acc) * diff->stars * 42.1f;

                        fieldsW[0]->set_text(fmt::format("{0} <size=2.5>({1:0.0f})</size>", (int)pp, acc));
						fieldsW[1]->set_text(fmt::format("{:1.1f}", diff->stars));
                    }
                }
            } else if (!BetterSongList::SongDetails::get_finishedInitAttempt()){
                INFO("details available, but empty and not yet fetched");
                fieldsW[0]->set_text("...");
                fieldsW[1]->set_text("...");
            }

            auto njs = selectedDifficultyBeatmap->get_noteJumpMovementSpeed();
            if (njs == 0) {
                njs = GlobalNamespace::BeatmapDifficultyMethods::NoteJumpMovementSpeed(selectedDifficultyBeatmap->get_difficulty());
            }

            INFO("Setting njs {}", njs);
            fieldsW[2]->set_text(fmt::format("{:1.1f}", njs));

            if (config.showMapJDInsteadOfOffset) { // map jump distance
                float jumpDistance = BetterSongList::JumpDistanceCalculator::GetJd(selectedDifficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_beatsPerMinute(), njs, selectedDifficultyBeatmap->get_noteJumpStartBeatOffset());
                fieldsW[3]->set_text(fmt::format("{:1.1f}", jumpDistance));
                INFO("Setting jumpDistance {}", jumpDistance);
            } else { // offset
                float offset = selectedDifficultyBeatmap->get_noteJumpStartBeatOffset();
                fieldsW[3]->set_text(fmt::format("{:1.1f}", offset));
                INFO("Setting offset  {}", offset);
            }
        } else {
            ERROR("Fields was nullptr!");
        }
    }

    void ExtraLevelParams::UpdateState() {
        if (lastInstance && lastInstance->get_isActiveAndEnabled()) lastInstance->RefreshContent();
    }

    void ExtraLevelParams::ModifyValue(TMPro::TextMeshProUGUI* text, std::string_view hoverHint, std::string_view icon) {
        DEBUG("Get go");
        auto go = text->get_gameObject();
        BSML::Utilities::SetImage(text->get_transform()->get_parent()->Find("Icon")->get_gameObject()->GetComponent<HMUI::ImageView*>(), icon);
        DEBUG("get hhint");
        auto localizedhhint = go->GetComponentInParent<GlobalNamespace::LocalizedHoverHint*>();
        if (localizedhhint) UnityEngine::Object::DestroyImmediate(localizedhhint);

        DEBUG("get hhint");
        auto hhint = go->GetComponentInParent<HMUI::HoverHint*>();

        if (!hhint) return;

        if (!hoverHintController) 
            hoverHintController = UnityEngine::Object::FindObjectOfType<HMUI::HoverHintController*>();
        hhint->hoverHintController = hoverHintController.ptr();
        hhint->set_text(hoverHint);
    }

    custom_types::Helpers::Coroutine ExtraLevelParams::ProcessFields() {
        if (!fields) {
            ERROR("Fields were not set, returning");
            co_return;
        }
        
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());
        ArrayW<TMPro::TextMeshProUGUI*> fieldsW{fields.ptr()};
        DEBUG("Fields length: {}", fieldsW.size());

        DEBUG("fields 0");
        ModifyValue(fieldsW[0], "ScoreSaber PP Value", "#DifficultyIcon");
        DEBUG("fields 1");
		ModifyValue(fieldsW[1], "ScoreSaber Star Rating", "#FavoritesIcon");
        DEBUG("fields 2");
		ModifyValue(fieldsW[2], "NJS (Note Jump Speed)", "#FastNotesIcon");
        DEBUG("fields 3");
		ModifyValue(fieldsW[3], "JD (Jump Distance, how close notes spawn)", "#MeasureIcon");

        DEBUG("richText");
        fieldsW[0]->set_richText(true);
        fieldsW[0]->set_characterSpacing(-3.0f);
        fieldsW[3]->set_richText(true);
    }
}
