#include "Sorters/Models/FolderDateSorter.hpp"
#include "logging.hpp"

#include "Utils/SongListLegendBuilder.hpp"

#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/IBeatmapLevelPackCollection.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

#include "songloader/shared/API.hpp"
#include "songloader/include/CustomTypes/SongLoader.hpp" 
#include "songloader/include/Utils/FindComponentsUtils.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include <sys/stat.h>
#include <time.h>

static const float MONTH_SECS = 1.0f / (60 * 60 * 24 * 30.4f);

namespace BetterSongList {
    std::map<std::string, int> FolderDateSorter::songTimes;

    bool FolderDateSorter::isLoading = false;

    FolderDateSorter::FolderDateSorter() : ISorterWithLegend(), ISorterPrimitive() {}

    bool FolderDateSorter::get_isReady() const {
        return !songTimes.empty();
    }

    std::future<void> FolderDateSorter::Prepare() {
        return Prepare(false);
    }

    void FolderDateSorter::OnSongsLoaded(const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
        Prepare(false);
    }

    void FolderDateSorter::GatherFolderInfoThread(bool fullReload) {
        std::string fpath = "/info.dat";

        GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel = nullptr; 
        // dispatch getting this thing on main thread
        QuestUI::MainThreadScheduler::Schedule([&beatmapLevelsModel](){
            // captured by reference so that's pogger
            beatmapLevelsModel = RuntimeSongLoader::FindComponentsUtils::GetBeatmapLevelsModel();
            if (!beatmapLevelsModel) {
                ERROR("Oh fuck we could not get beatmapLevelsModel: {}", fmt::ptr(beatmapLevelsModel));
            }
        });

        // wait for it to be set
        while (!beatmapLevelsModel) std::this_thread::yield();
        
        auto dict = beatmapLevelsModel->loadedPreviewBeatmapLevels;
        auto enumerator = dict->GetEnumerator();
        while(enumerator.MoveNext()) {
            auto level = enumerator.get_Current();
            auto customLevel = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level.get_Value()).value_or(nullptr);
            if (customLevel) {
                std::string levelID = customLevel->get_levelID();
                auto itr = songTimes.find(levelID);
                if (itr != songTimes.end() && !fullReload) continue;

                struct stat fileStat = {0};
                std::string filePath = customLevel->get_customLevelPath() + fpath;
                if (stat(filePath.c_str(), &fileStat) == 0) {
                    songTimes[levelID] = fileStat.st_ctim.tv_sec;
                }
            }
        }

        isLoading = false;
    }

    std::future<void> FolderDateSorter::Prepare(bool fullReload) {
        if (isLoading) {
            return std::async(std::launch::deferred, []{});
        }
        isLoading = true;
        return std::async(std::launch::async, [fullReload, this](){
            auto loader = RuntimeSongLoader::SongLoader::GetInstance();
            while(!loader->HasLoaded || loader->IsLoading) std::this_thread::yield();
            
            this->FolderDateSorter::GatherFolderInfoThread(fullReload);
        });
    }

    std::optional<float> FolderDateSorter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const {
		std::string levelId = level ? level->get_levelID() : "";
        if (levelId.empty()) return std::nullopt;
        auto itr = songTimes.find(levelId);
        if (itr != songTimes.end()) return itr->second;
        return std::nullopt;
    }

    ISorterWithLegend::Legend FolderDateSorter::BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const {
        time_t now = time(NULL);
        return SongListLegendBuilder::BuildFor(levels, [now](GlobalNamespace::IPreviewBeatmapLevel* level) -> std::string {
            std::string levelId = level ? level->get_levelID() : "";
            if (levelId.empty()) return "";
            auto itr = songTimes.find(levelId);
            if (itr == songTimes.end()) return "";
            auto months = (now - itr->second) * MONTH_SECS;

            if (months < 1.0f) return "<1 M";

            return fmt::format("{} M", (int)months);
        });
    }
}