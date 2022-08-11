#include "Patches/UI/ScrollEnhancement.hpp"
#include "config.hpp"

#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/Helpers/delegates.hpp"

#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"

#include "System/Tuple_2.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"
#include "HMUI/NoTransitionsButton.hpp"

struct FloatLiteral {
    constexpr FloatLiteral(const float& a) : v(a) {};
    constexpr operator float() const { return v; }
    float v;
};

template<FloatLiteral step, int direction>
void Scroll(HMUI::TableView* table) {
    auto cells = table->get_dataSource()->NumberOfCells();
    if (cells == 0) return;
    int amt = (float)cells * step * (float)direction;

    if constexpr ((step - 1) > 0.001f) {
        amt += table->GetVisibleCellsIdRange()->get_Item1();
    }

    table->ScrollToCellWithIdx(amt, HMUI::TableView::ScrollPositionType::Beginning, true);
}

namespace BetterSongList::Hooks {
    SafePtr<Array<UnityEngine::GameObject*>> ScrollEnhancement::buttons;

    void ScrollEnhancement::LevelCollectionTableView_Init_Prefix(GlobalNamespace::LevelCollectionTableView* self, bool isInitialized, HMUI::TableView* tableView) {
        if (!isInitialized) 
            GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(DoTheFunny(tableView, self->get_transform())));

        UpdateState();
    }

    void ScrollEnhancement::UpdateState() {
        if (!buttons) {
            ArrayW<UnityEngine::GameObject*> btns{buttons.ptr()};
            for (auto btn : btns) {
                if (btn && btn->m_CachedPtr.m_value) {
                    btn->SetActive(config.extendSongScrollbar);
                }
            }
        }
    }

    UnityEngine::Transform* ScrollEnhancement::BuildButton(UnityEngine::Transform* baseButton, StringW Icon, float vOffs, float rotation, std::function<void()> cb) {
        auto newBtnGo = UnityEngine::Object::Instantiate(baseButton->get_gameObject(), 
                baseButton
                ->get_parent() // ScrollBar
                ->get_parent() // LevelsTableView
                ->get_parent() // LevelCollectionViewController
            );
        auto newBtn = reinterpret_cast<UnityEngine::RectTransform*>(newBtnGo->get_transform());
        newBtn->set_anchorMin({0.96f, 0.893f - vOffs});
        newBtn->set_anchorMax({1, 0.953f - vOffs});

        auto i = newBtn->GetComponentInChildren<HMUI::ImageView*>();
        BSML::Utilities::SetImage(i, Icon);

        auto iT = i->get_rectTransform();
        iT->set_offsetMax({0, 0});
        iT->set_offsetMin({-2.5f, -2.5f});
        iT->set_localEulerAngles({0, 0, rotation});

        auto btn = newBtnGo->GetComponent<HMUI::NoTransitionsButton*>();
        btn->set_interactable(true);
        auto btnClickedEvent = UnityEngine::UI::Button::ButtonClickedEvent::New_ctor();
        btnClickedEvent->AddListener(BSML::MakeUnityAction(cb));
        btn->set_onClick(btnClickedEvent);

        return newBtn;
    }

    custom_types::Helpers::Coroutine ScrollEnhancement::DoTheFunny(HMUI::TableView* table, UnityEngine::Transform* a) {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());

        auto r = reinterpret_cast<UnityEngine::RectTransform*>(table->get_transform()->get_parent()->get_parent());
        r->set_sizeDelta({4, 0});

        r = reinterpret_cast<UnityEngine::RectTransform*>(table->get_transform()->get_parent());
        auto anchorMin = r->get_anchorMin();
        anchorMin.x += 0.02f;
        auto sizeDelta = r->get_sizeDelta();
        sizeDelta.x -= 2.0f;
        r->set_anchorMin(anchorMin);
        r->set_sizeDelta(sizeDelta);
        static ConstString btnPath{"ScrollBar/UpButton"};
        auto button = a->Find(btnPath);

        auto btnUpFast = BuildButton(button, MOD_ID "_double_arrow", 0, -90, [table](){Scroll<FloatLiteral(0.1f), -1>(table);});
        auto btnDownFast = BuildButton(button, MOD_ID "_double_arrow", 0.86f, 90, [table](){Scroll<FloatLiteral(0.1f), 1>(table);});

        auto btnArr = ArrayW<UnityEngine::GameObject*>(il2cpp_array_size_t(4));
        btnArr[0] = btnUpFast->get_gameObject();
        btnArr[1] = BuildButton(button, "#HeightIcon", 0.09f, 0, [table](){Scroll<FloatLiteral(1.0f), 0>(table);})->get_gameObject();
        btnArr[2] = BuildButton(button, "#HeightIcon", 0.77f, 180, [table](){Scroll<FloatLiteral(1.0f), 1>(table);})->get_gameObject();
        btnArr[3] = btnDownFast->get_gameObject();
    }
}
