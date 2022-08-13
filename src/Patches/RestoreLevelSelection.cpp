#include "Patches/RestoreLevelSelection.hpp"
#include "Patches/RestoreTableScroll.hpp"
#include "Patches/HookLevelCollectionTableSet.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Sprite.hpp"
#include "GlobalNamespace/IBeatmapLevelPackCollection.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"

#include "Utils/PlaylistUtils.hpp"

#include "logging.hpp"
#include "config.hpp"

#include "songloader/include/Utils/FindComponentsUtils.hpp"

namespace BetterSongList::Hooks {
    void RestoreLevelSelection::LevelSelectionFlowCoordinator_DidActivate_Prefix(GlobalNamespace::LevelSelectionFlowCoordinator::State*& startState) {
        if (startState) {
            return;
        }

        auto restoreCategory = config.lastCategory;
        auto restoreLevel = config.lastSong;
        GlobalNamespace::IPreviewBeatmapLevel* m = nullptr;

        if (!restoreLevel.empty()) {
            auto bm = RuntimeSongLoader::FindComponentsUtils::GetBeatmapLevelsModel();
            auto packCollection = bm->get_allLoadedBeatmapLevelPackCollection();
            auto packs = packCollection->get_beatmapLevelPacks();

            for (auto pack : packs) {
                auto levelCollection = pack->i_IAnnotatedBeatmapLevelCollection()->get_beatmapLevelCollection();
                ListWrapper<GlobalNamespace::IPreviewBeatmapLevel*> levels{levelCollection->get_beatmapLevels()};
                for (auto level : levels) {
                    if (level->get_levelID() != restoreLevel) {
                        continue;
                    }
                    m = level;
                    break;
                }
            }
        }

        auto lastPack = PlaylistUtils::GetPack(config.lastPack);
        startState = GlobalNamespace::LevelSelectionFlowCoordinator::State::New_ctor(
            System::Nullable_1<LevelCategory>(restoreCategory, true),
            GlobalNamespace::BeatmapLevelPack::New_ctor(
                lastPack ? lastPack->get_packID() : nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            )->i_IBeatmapLevelPack(),
            m,
            nullptr
        );
    }
}