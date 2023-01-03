#include "Patches/UI/SongDeleteButton.hpp"
#include "config.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/delegates.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"

#include "songloader/shared/API.hpp"

namespace BetterSongList::Hooks {
    SafePtr<GlobalNamespace::CustomPreviewBeatmapLevel> SongDeleteButton::lastLevel;
    SafePtrUnity<UnityEngine::UI::Button> SongDeleteButton::deleteButton;

    GlobalNamespace::CustomPreviewBeatmapLevel* SongDeleteButton::get_lastLevel() {
        if (!lastLevel) return nullptr;
        return lastLevel.ptr();
    }
    UnityEngine::UI::Button* SongDeleteButton::get_deleteButton() {
        if (!deleteButton) return nullptr;
        return deleteButton.ptr();
    }

    void SongDeleteButton::StandardLevelDetailView_RefreshContent_Postfix(GlobalNamespace::StandardLevelDetailView* self, UnityEngine::UI::Button* practiceButton, GlobalNamespace::IPreviewBeatmapLevel* level) {
        if (!get_deleteButton() && practiceButton) {
            auto newButton = UnityEngine::Object::Instantiate(practiceButton->get_gameObject(), practiceButton->get_transform()->get_parent());
            deleteButton = newButton->GetComponentInChildren<UnityEngine::UI::Button*>();
            auto deleteConfirmHandlerInstance = DeleteConfirmHandler::get_instance();

            auto onClick = UnityEngine::UI::Button::ButtonClickedEvent::New_ctor();
            auto klass = deleteConfirmHandlerInstance->klass;
            auto minfo = il2cpp_functions::class_get_method_from_name(klass, "Confirm", 0);
            auto action = BSML::MakeUnityAction(deleteConfirmHandlerInstance, minfo);
            onClick->AddListener(action);
            deleteButton->set_onClick(onClick);

            newButton->GetComponentsInChildren<UnityEngine::UI::LayoutElement*>().LastOrDefault()->set_minWidth(12.0f);
            newButton->get_transform()->SetAsFirstSibling();

            auto t = newButton->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
            auto iconG = UnityEngine::GameObject::New_ctor("Icon");
            iconG->get_transform()->SetParent(t->get_transform()->get_parent(), false);
            iconG->get_transform()->set_localScale({0.69f, 0.69f, 0.0f});
            auto icon = iconG->AddComponent<HMUI::ImageView*>();
            icon->set_color(t->get_color());
            icon->skew = 0.18f;
            icon->set_material(BSML::Helpers::GetUINoGlowMat());
            BSML::Utilities::SetImage(icon, "#DeleteIcon");

            UnityEngine::Object::DestroyImmediate(t->get_gameObject());

            BSML::parse_and_construct(IncludedAssets::SongDeleteConfirm_bsml, self->get_transform()->get_parent(), deleteConfirmHandlerInstance);
        }

        auto casted_level = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level).value_or(nullptr);
        if (casted_level) lastLevel = casted_level;

        UpdateState();
    }

    void SongDeleteButton::UpdateState() {
        if (!get_deleteButton() || !get_deleteButton()->m_CachedPtr.m_value) return;

        deleteButton->set_interactable(lastLevel && (config.get_allowWipDelete() || !get_isWip()));
    }

    bool SongDeleteButton::get_isWip() {
        if (!get_lastLevel()) return false;
        auto levelId = static_cast<std::u16string_view>(lastLevel->get_levelID());
        return levelId.find(u" WIP") != std::u16string_view::npos;
    }
}

DEFINE_TYPE(BetterSongList, DeleteConfirmHandler);

namespace BetterSongList {
    SafePtr<DeleteConfirmHandler> DeleteConfirmHandler::instance;

    DeleteConfirmHandler* DeleteConfirmHandler::get_instance() {
        if (!instance || !instance.ptr()) {
            instance.emplace(DeleteConfirmHandler::New_ctor());
        }

        return instance.ptr();
    }

    void DeleteConfirmHandler::ConfirmDelete() {
        if (deleteModal && deleteModal->m_CachedPtr.m_value)
            deleteModal->Show();
    }

    void DeleteConfirmHandler::Confirm() {
        auto lastLevel = BetterSongList::Hooks::SongDeleteButton::get_lastLevel();
        if (!lastLevel) return;

        RuntimeSongLoader::API::DeleteSong(static_cast<std::string>(lastLevel->get_customLevelPath()));

        if (!BetterSongList::Hooks::SongDeleteButton::get_isWip()) {
            return;
        }

        if (deleteModal && deleteModal->m_CachedPtr.m_value)
            deleteModal->Hide();
    }
}