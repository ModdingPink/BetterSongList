#include "Sorters/Models/FolderDateSorter.hpp"
#include "logging.hpp"

#include "Utils/SongListLegendBuilder.hpp"

#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/IBeatmapLevelPackCollection.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

#include "songloader/shared/API.hpp"
#include "songloader/include/CustomTypes/SongLoader.hpp" 
#include "songloader/include/Utils/FindComponentsUtils.hpp"

#include <sys/stat.h>
#include <time.h>

static const float MONTH_SECS = 1.0f / (60 * 60 * 24 * 30.4f);

namespace BetterSongList {
    SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> FolderDateSorter::loadingTask;
    std::map<std::string, int> FolderDateSorter::songTimes;

    bool FolderDateSorter::isLoading = false;

    FolderDateSorter::FolderDateSorter() : ISorterWithLegend(), ISorterPrimitive() {}

    bool FolderDateSorter::get_isReady() const {
        return !songTimes.empty();
    }

    System::Threading::Tasks::Task* FolderDateSorter::Prepare() {
        return Prepare(false);
    }

    void FolderDateSorter::OnSongsLoaded(const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
        Prepare(false);
    }

    void FolderDateSorter::GatherFolderInfoThread(bool fullReload) {
        std::string fpath = "/info.dat";

        auto beatmapLevelsModel = RuntimeSongLoader::FindComponentsUtils::GetBeatmapLevelsModel();
        auto collections = beatmapLevelsModel->get_allLoadedBeatmapLevelPackCollection();
        auto packs = collections->get_beatmapLevelPacks();

        for (auto pack : packs) {
            auto customPack = il2cpp_utils::try_cast<RuntimeSongLoader::SongLoaderCustomBeatmapLevelPack>(pack).value_or(nullptr);
            if (!customPack) continue;
            auto collection = customPack->CustomLevelsCollection;
            ListWrapper<GlobalNamespace::IPreviewBeatmapLevel*> levels{collection->get_beatmapLevels()};
            for (auto level : levels) {
                auto customLevel = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level).value_or(nullptr);
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
        }

        loadingTask->TrySetResult(true);
		loadingTask.emplace(nullptr);
        isLoading = false;
    }

    System::Threading::Tasks::Task* FolderDateSorter::Prepare(bool fullReload) {
        if (songTimes.empty()) {
            RuntimeSongLoader::API::AddSongsLoadedEvent(std::bind(&FolderDateSorter::OnSongsLoaded, this, std::placeholders::_1));
        }

        if (!loadingTask) {
            loadingTask = System::Threading::Tasks::TaskCompletionSource_1<bool>::New_ctor();
        }

        auto loader = RuntimeSongLoader::SongLoader::GetInstance();

        if (!loader->HasLoaded || loader->IsLoading) {
            return reinterpret_cast<System::Threading::Tasks::Task*>(loadingTask->get_Task());
        }

        if (!isLoading) {
            isLoading = true;
            std::thread(std::bind(&FolderDateSorter::GatherFolderInfoThread, this, fullReload)).detach();
        }

        return reinterpret_cast<System::Threading::Tasks::Task*>(loadingTask->get_Task());
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