#pragma once

#include "IFilter.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include <vector>

namespace BetterSongList {
    class RequirementsFilter : public IFilter {
        public:
            RequirementsFilter();

            virtual bool get_isReady() const override;
            virtual std::future<void> Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
        private:
            static bool inited;
    };
}