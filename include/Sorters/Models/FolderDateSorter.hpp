#pragma once

#include "ISorter.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include <map>

namespace BetterSongList {
    class FolderDateSorter : public ISorterWithLegend, public ISorterPrimitive {
        public:
            FolderDateSorter();
            virtual bool get_isReady() const override;
            virtual std::future<void> Prepare() override;
            std::future<void> Prepare(bool fullReload);
            virtual std::optional<float> GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const override;
            virtual Legend BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const override;
        private:
            void OnSongsLoaded(const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs);
            void GatherFolderInfoThread(bool fullReload = false);
            static std::map<std::string, int> songTimes;
            static bool isLoading;
    };
}