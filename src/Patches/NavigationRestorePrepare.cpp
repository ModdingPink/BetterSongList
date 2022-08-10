#include "Patches/NavigationRestorePrepare.hpp"
#include "Utils/PlaylistUtils.hpp"
#include "logging.hpp"
#include "config.hpp"

namespace BetterSongList::Hooks {
    std::string NavigationRestorePrepare::levelPack;
    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory NavigationRestorePrepare::category = GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None;
    std::string NavigationRestorePrepare::level;

    std::string_view NavigationRestorePrepare::get_collection() {
        auto v = levelPack;
        levelPack.clear();
        return v;
    }

    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory NavigationRestorePrepare::get_category() {
        auto v = category;
        category = GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None;
        return v;
    }

    std::string_view NavigationRestorePrepare::get_level() {
        auto v = level;
        level.clear();
        return v;
    }

    void NavigationRestorePrepare::Prefix(GlobalNamespace::LevelSelectionFlowCoordinator* self) {
        auto startState = self->startState;
        if (startState && (startState->beatmapLevelPack || startState->previewBeatmapLevel || startState->levelCategory.value != GlobalNamespace::SelectLevelCategoryViewController::LevelCategory::None)) {
            return;
        }

        category = config.lastCategory;
        auto pack = PlaylistUtils::GetPack(config.lastPack);
        levelPack = pack ? static_cast<std::string>(pack->get_packID()) : "";
        level = config.lastSong;
    }
}
