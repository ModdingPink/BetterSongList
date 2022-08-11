#pragma once

#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/ClickableText.hpp"
#include "bsml/shared/BSML/Components/ClickableImage.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "HMUI/DropdownWithTableView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "Sorters/Models/ISorter.hpp"
#include "Filters/Models/IFilter.hpp"

#include <queue>

DECLARE_CLASS_CODEGEN(BetterSongList, FilterUI, Il2CppObject,
    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, root);
    DECLARE_INSTANCE_FIELD(HMUI::DropdownWithTableView*, sortDropDown);
    DECLARE_INSTANCE_FIELD(HMUI::DropdownWithTableView*, filterDropDown);
    DECLARE_INSTANCE_FIELD(ListWrapper<Il2CppObject*>, sortOptionsList);
    DECLARE_INSTANCE_FIELD(ListWrapper<Il2CppObject*>, filterOptionsList);
    DECLARE_INSTANCE_FIELD(bool, warningLoadInProgress);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, filterLoadingIndicator);
    DECLARE_INSTANCE_FIELD(BSML::ClickableText*, sortDirection);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, failTextLabel);
    DECLARE_INSTANCE_FIELD(bool, settingsWereOpened);
    DECLARE_INSTANCE_FIELD(BSML::ClickableText*, settingsButton);

    DECLARE_INSTANCE_METHOD(void, UpdateDropDowns);
    DECLARE_INSTANCE_METHOD(void, UpdateTransformerOptionsAndDropdowns);
    DECLARE_INSTANCE_METHOD(void, ToggleSortDirection);
    DECLARE_INSTANCE_METHOD(void, SelectRandom);
    DECLARE_INSTANCE_METHOD(void, _SetSort, StringW selected);
    DECLARE_INSTANCE_METHOD(void, _SetFilter, StringW selected);
    DECLARE_INSTANCE_METHOD(void, PossiblyShowNextWarning);
    DECLARE_INSTANCE_METHOD(void, SettingsOpened);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    
    DECLARE_DEFAULT_CTOR();
    public:
		static void UpdateVisibleTransformers();
        static FilterUI* get_instance();
		static void SetSort(std::string selected, bool storeToConfig = true, bool refresh = true);
		static void ClearFilter(bool reloadTable = false);
		static void SetFilter(std::string selected, bool storeToConfig = true, bool refresh = true);
		static void SetSortDirection(bool ascending, bool refresh = true);
		static void Init();
		static void AttachTo(UnityEngine::Transform* target);
		static void HackDropdown(HMUI::DropdownWithTableView* dropdown);
        void ShowErrorASAP(StringW text = nullptr);

    private:
        static SafePtr<FilterUI> instance;
        static std::map<std::string, BetterSongList::ISorter*> sortOptions;
        static std::map<std::string, BetterSongList::IFilter*> filterOptions;
        
        std::shared_ptr<BSML::BSMLParser> parser;
        std::queue<std::string> warnings;

        custom_types::Helpers::Coroutine _ShowError();
        custom_types::Helpers::Coroutine PossiblyDrawUserAttentionToSettingsButton();
)