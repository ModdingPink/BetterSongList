#include "Utils/PlaylistUtils.hpp"
#include "modloader/shared/modloader.hpp"

#include "UnityEngine/Object.hpp"
#include "GlobalNamespace/CustomBeatmapLevelPack.hpp"
#include "GlobalNamespace/CustomBeatmapLevelCollection.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"

#include "songloader/include/CustomTypes/SongLoader.hpp"
#include "songloader/include/Utils/FindComponentsUtils.hpp"

namespace BetterSongList::PlaylistUtils {
    static bool hasPlaylistLib = false;
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
        // TODO: check if right name
        hasPlaylistLib = !Modloader::requireMod("playlistcore");
    }

    GlobalNamespace::IBeatmapLevelPack* GetPack(StringW packName) {
        if (!packName) return nullptr;
        auto songLoader = RuntimeSongLoader::SongLoader::GetInstance();
        if (packName == "Custom Levels") {
            return songLoader->CustomLevelsPack->CustomLevelsPack->i_IBeatmapLevelPack();
        } else if (packName == "WIP Levels") {
            return songLoader->CustomWIPLevelsPack->CustomLevelsPack->i_IBeatmapLevelPack();
        }

        if (get_builtinPacks() == nullptr) {
            auto levelsModel = RuntimeSongLoader::FindComponentsUtils::GetBeatmapLevelsModel();
            auto collection = levelsModel->get_allLoadedBeatmapLevelWithoutCustomLevelPackCollection();
            auto packs = collection->get_beatmapLevelPacks();
            builtinPacks = Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>::New_ctor();

            for (auto p : packs) {
                builtinPacks->Add(p->get_shortPackName(), p);
            }
        }

        GlobalNamespace::IBeatmapLevelPack* v;
        if (get_builtinPacks()->TryGetValue(packName, byref(v))) {
            return v;
        } else if (hasPlaylistLib) {
            /*
            TODO: Playlist lib tie-in. if playlist stuff is not core symbol lookups?
            auto playlists = playlistlib.getallplaylists;
            for (auto p : playlists) {
                if (p->get_packName() == packName) {
                    return p;
                }
            }
            */
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