#include "Patches/HookSelectedCollection.hpp"
#include "config.hpp"

#include "songloader/include/CustomTypes/SongLoader.hpp"
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
        auto songLoader = RuntimeSongLoader::SongLoader::GetInstance();
        auto pack = songLoader->CustomLevelsPack->CustomLevelsPack->i_IBeatmapLevelPack();

        // TODO: a bunch of filterui stuff
        if (beatmapLevelCollection && config.clearFiltersOnPlaylistSelect && beatmapLevelCollection != pack->i_IAnnotatedBeatmapLevelCollection()) {
            //FilterUI.SetSort(null, false, false);
			//FilterUI.SetFilter(null, false, false);
        } else if (lastSelectedCollection) {
            //FilterUI.SetSort(config.lastSort, false, false);
			//FilterUI.SetFilter(config.lastFilter, false, false);
        }

        lastSelectedCollection.emplace(beatmapLevelCollection);
        //FilterUI.persistentNuts?.UpdateTransformerOptionsAndDropdowns();
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