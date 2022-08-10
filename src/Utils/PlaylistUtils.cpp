#include "Utils/PlaylistUtils.hpp"
#include "modloader/shared/modloader.hpp"

#include "UnityEngine/Object.hpp"
#include "GlobalNamespace/CustomBeatmapLevelPack.hpp"
#include "GlobalNamespace/CustomBeatmapLevelCollection.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"

namespace BetterSongList::PlaylistUtils {
    static bool hasPlaylistLib = false;
    bool get_hasPlaylistLib() {
        return hasPlaylistLib;
    }

    static bool requiresListCast = false;
    bool get_requiresListCast() {
        return requiresListCast;
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
        requiresListCast = false;
    }

    GlobalNamespace::IBeatmapLevelPack* GetPack(StringW packName) {
        if (!packName) return nullptr;
        if (packName == "Custom Levels") {
            //return SongLoader::CustomLevelsPack 
        } else if (packName == "WIP Levels") {
            //return SongLoader::WIP levels
        }

        if (get_builtinPacks() != nullptr) {
            // TODO: make this actually a thing that exists
            //builtinPacks = songloader get all packs?
            builtinPacks = Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>::New_ctor();
        }

        GlobalNamespace::IBeatmapLevelPack* v;
        if (get_builtinPacks()->TryGetValue(packName, byref(v))) {
            return v;
        } else if (hasPlaylistLib) {
            /*
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
        auto customBeatmapLevelPack = il2cpp_utils::try_cast<GlobalNamespace::CustomBeatmapLevelPack>(levelCollection);
        if (customBeatmapLevelPack.has_value()) {
            return customBeatmapLevelPack.value()->get_beatmapLevelCollection()->get_beatmapLevels();
        }
        return nullptr;
    }
}