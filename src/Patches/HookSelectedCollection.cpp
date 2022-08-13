#include "Patches/HookSelectedCollection.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "songloader/include/CustomTypes/SongLoader.hpp"

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
            auto name = beatmapLevelCollection->get_collectionName();
            config.lastPack = name ? static_cast<std::string>(name) : "";
        }

        WARNING("AnnotatedBeatmapLevelCollectionsViewController.HandleDidSelectAnnotatedBeatmapLevelCollection(): {0}", beatmapLevelCollection ? beatmapLevelCollection->get_collectionName() : "NULL");
        auto songLoader = RuntimeSongLoader::SongLoader::GetInstance();
        auto customLevelspack = songLoader->CustomLevelsPack;
        auto actualPack = customLevelspack ? customLevelspack->CustomLevelsPack : nullptr;
        auto pack = actualPack ? actualPack->i_IBeatmapLevelPack()->i_IAnnotatedBeatmapLevelCollection() : nullptr;

        auto instance = FilterUI::get_instance();
        if (beatmapLevelCollection && config.clearFiltersOnPlaylistSelect && beatmapLevelCollection != pack) {
            instance->SetSort("", false, false);
            instance->SetFilter("", false, false);
        } else if (lastSelectedCollection) {
            instance->SetSort(config.lastSort, false, false);
            instance->SetFilter(config.lastFilter, false, false);
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