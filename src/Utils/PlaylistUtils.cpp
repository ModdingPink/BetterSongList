#include "Utils/PlaylistUtils.hpp"
#include "modloader/shared/modloader.hpp"
#include "logging.hpp"

#include "UnityEngine/Object.hpp"
#include "GlobalNamespace/CustomBeatmapLevelPack.hpp"
#include "GlobalNamespace/CustomBeatmapLevelCollection.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"

#include "songloader/include/CustomTypes/SongLoader.hpp"
#include "songloader/shared/API.hpp"
#include "songloader/include/Utils/FindComponentsUtils.hpp"

#include "playlistcore/shared/PlaylistCore.hpp"
namespace BetterSongList::PlaylistUtils {
    // we link to the playlist lib so it's always available
    static bool hasPlaylistLib = true;
    bool get_hasPlaylistLib() {
        return hasPlaylistLib;
    }

    SafePtr<Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>> builtinPacks;
    Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>* get_builtinPacks() {
        if (!builtinPacks) {
            return nullptr;
        }
        return builtinPacks.ptr();
    }

    void Init() {
        INFO("HAHABALL we don't need to init anything cause playlist mod is required on quest :)");
    }

    GlobalNamespace::IBeatmapLevelPack* GetPack(StringW packID) {
        if (!packID) return nullptr;
        auto songLoader = RuntimeSongLoader::SongLoader::GetInstance();
        static std::string customLevelPackId = RuntimeSongLoader::API::GetCustomLevelPacksPrefix() + "Custom_Levels";
        static std::string customWipLevelPackId = RuntimeSongLoader::API::GetCustomLevelPacksPrefix() + "Custom_Levels";
        if (packID == customLevelPackId) {
            return songLoader->CustomLevelsPack->CustomLevelsPack->i_IBeatmapLevelPack();
        } else if (packID == customWipLevelPackId) {
            return songLoader->CustomWIPLevelsPack->CustomLevelsPack->i_IBeatmapLevelPack();
        }

        if (!get_builtinPacks()) {
            auto levelsModel = RuntimeSongLoader::FindComponentsUtils::GetBeatmapLevelsModel();
            auto collection = levelsModel->get_allLoadedBeatmapLevelWithoutCustomLevelPackCollection();
            auto packs = collection->get_beatmapLevelPacks();
            builtinPacks = Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>::New_ctor();

            for (auto p : packs) {
                builtinPacks->Add(p->get_packID(), p);
            }
        }

        GlobalNamespace::IBeatmapLevelPack* v = nullptr;
        if (get_builtinPacks()->TryGetValue(packID, byref(v))) {
            return v;
        } else if (hasPlaylistLib) {
            auto pls = PlaylistCore::GetLoadedPlaylists();
            for (auto pl : pls) {
                if (pl->playlistCS) {
                    if (pl->playlistCS->get_packID() == packID) {
                        return pl->playlistCS->i_IBeatmapLevelPack();
                    }
                }
            }
        }

        return nullptr;
    }

    bool IsCollection(GlobalNamespace::IAnnotatedBeatmapLevelCollection* levelCollection) {
        // TODO: check if these checks are even right because it's really confusing how songloader does playlists
        return il2cpp_utils::try_cast<GlobalNamespace::CustomBeatmapLevelPack>(levelCollection).has_value();
    }

    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> GetLevelsForLevelCollection(GlobalNamespace::IAnnotatedBeatmapLevelCollection* levelCollection) {
        // TODO: idem
        auto collection = levelCollection ? levelCollection->get_beatmapLevelCollection() : nullptr;
        return collection ? collection->get_beatmapLevels() : ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>(il2cpp_array_size_t(0));
    }
}