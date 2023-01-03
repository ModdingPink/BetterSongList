#include "Patches/HookSelectedCollection.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "songloader/include/CustomTypes/SongLoader.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"

#include "UI/FilterUI.hpp"

namespace BetterSongList::Hooks {
    SafePtr<GlobalNamespace::IAnnotatedBeatmapLevelCollection> HookSelectedCollection::lastSelectedCollection;

    GlobalNamespace::IAnnotatedBeatmapLevelCollection* HookSelectedCollection::get_lastSelectedCollection() {
        if (!lastSelectedCollection) {
            return nullptr;
        }
        return lastSelectedCollection.ptr();
    }

    // same as CollectionSet
    void HookSelectedCollection::AnnotatedBeatmapLevelCollectionsViewController_HandleDidSelectAnnotatedBeatmapLevelCollection_Prefix(GlobalNamespace::IAnnotatedBeatmapLevelCollection* beatmapLevelCollection) {
        if (beatmapLevelCollection) {
            INFO("Setting last selected pack");
            auto beatmapLevelPack = il2cpp_utils::try_cast<GlobalNamespace::IBeatmapLevelPack>(beatmapLevelCollection);
            auto name = beatmapLevelPack.has_value() ? beatmapLevelPack.value()->get_packID() : nullptr;
            config.set_lastPack(name ? static_cast<std::string>(name) : "");
            INFO("It is now '{}'", config.get_lastPack());
        }

        INFO("AnnotatedBeatmapLevelCollectionsViewController.HandleDidSelectAnnotatedBeatmapLevelCollection(): {0}", beatmapLevelCollection ? beatmapLevelCollection->get_collectionName() : "NULL");
        auto songLoader = RuntimeSongLoader::SongLoader::GetInstance();
        auto customLevelspack = songLoader->CustomLevelsPack;
        auto actualPack = customLevelspack ? customLevelspack->CustomLevelsPack : nullptr;
        auto pack = actualPack ? actualPack->i_IBeatmapLevelPack()->i_IAnnotatedBeatmapLevelCollection() : nullptr;

        auto instance = FilterUI::get_instance();
        if (beatmapLevelCollection && config.get_clearFiltersOnPlaylistSelect() && beatmapLevelCollection != pack) {
            instance->SetSort("", false, false);
            instance->SetFilter("", false, false);
        } else if (get_lastSelectedCollection()) {
            instance->SetSort(config.get_lastSort(), false, false);
            instance->SetFilter(config.get_lastFilter(), false, false);
        }

        lastSelectedCollection.emplace(beatmapLevelCollection);
        instance->UpdateTransformerOptionsAndDropdowns();
        DEBUG("EOF HandleDidSelectAnnotatedBeatmapLevelCollection:Prefix");
    }

    // HookLevelCollectionUnset
    void HookSelectedCollection::LevelFilteringNavigationController_UpdateSecondChildControllerContent_Prefix(GlobalNamespace::SelectLevelCategoryViewController::LevelCategory levelCategory) {
        if (levelCategory != GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::CustomSongs) {
            AnnotatedBeatmapLevelCollectionsViewController_HandleDidSelectAnnotatedBeatmapLevelCollection_Prefix(nullptr);
        }
    }

    // HookLevelCollectionInit
    void HookSelectedCollection::AnnotatedBeatmapLevelCollectionsViewController_SetData_PostFix(GlobalNamespace::AnnotatedBeatmapLevelCollectionsViewController* self) {
        AnnotatedBeatmapLevelCollectionsViewController_HandleDidSelectAnnotatedBeatmapLevelCollection_Prefix(self->get_selectedAnnotatedBeatmapLevelCollection());
    }

}