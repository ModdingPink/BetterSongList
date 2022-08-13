#pragma once

#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IAnnotatedBeatmapLevelCollection.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"

namespace BetterSongList::PlaylistUtils {
    template<typename T, typename U>
    using Dictionary = System::Collections::Generic::Dictionary_2<T, U>;
    
    bool get_hasPlaylistLib();
    Dictionary<StringW, GlobalNamespace::IBeatmapLevelPack*>* get_builtinPacks();

    void Init();
    GlobalNamespace::IBeatmapLevelPack* GetPack(StringW packName);
    bool IsCollection(GlobalNamespace::IAnnotatedBeatmapLevelCollection* levelCollection);
    ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> GetLevelsForLevelCollection(GlobalNamespace::IAnnotatedBeatmapLevelCollection* levelCollection);
}