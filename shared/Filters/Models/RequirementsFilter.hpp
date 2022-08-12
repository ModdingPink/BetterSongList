#pragma once

#include "IFilter.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include <vector>

namespace BetterSongList {
    class RequirementsFilter : public IFilter {
        public:
            RequirementsFilter();

            virtual bool get_isReady() const override;
            virtual void Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
            void SongsLoadedCallback(const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs);
        private:
            static bool inited;
    };
}