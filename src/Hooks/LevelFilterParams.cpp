#include "hooking.hpp"
#include "config.hpp"

#include "GlobalNamespace/LevelFilterParams.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficultyMask.hpp"
#include "GlobalNamespace/SongPackMask.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"

// from PresetFilter
MAKE_AUTO_HOOK_FIND_INSTANCE(LevelFilterParams_ctor_args,
    classof(GlobalNamespace::LevelFilterParams*),
    ".ctor",
    void,
    GlobalNamespace::LevelFilterParams* self,
    bool filterByLevelIds,
    ::System::Collections::Generic::HashSet_1<::StringW>* beatmapLevelIds,
    ::StringW searchText,
    bool filterByOwned,
    bool filterByDifficulty,
    ::GlobalNamespace::BeatmapDifficultyMask filteredDifficulty,
    bool filterByCharacteristic,
    ::GlobalNamespace::BeatmapCharacteristicSO* filteredCharacteristic,
    bool filterBySongPacks,
    ::GlobalNamespace::SongPackMask filteredSongPacks,
    bool filterByNotPlayedYet,
    bool filterByMinBpm,
    float filteredMinBpm,
    bool filterByMaxBpm,
    float filteredMaxBpm) {
        LevelFilterParams_ctor_args(
            self,
            filterByLevelIds,
            beatmapLevelIds,
            searchText,
            filterByOwned || config.autoFilterUnowned, /* <-this is why we have to hook this ctor*/
            filterByDifficulty,
            filteredDifficulty,
            filterByCharacteristic,
            filteredCharacteristic,
            filterBySongPacks,
            filteredSongPacks,
            filterByNotPlayedYet,
            filterByMinBpm,
            filteredMinBpm,
            filterByMaxBpm,
            filteredMaxBpm
        );
}

// from PresetFilter
MAKE_AUTO_HOOK_FIND_INSTANCE(LevelFilterParams_ctor_0,
    classof(GlobalNamespace::LevelFilterParams*),
    ".ctor",
    void,
    GlobalNamespace::LevelFilterParams* self) {
        LevelFilterParams_ctor_0(self);
        self->set_filterByOwned(self->get_filterByOwned() || config.autoFilterUnowned);
    }
