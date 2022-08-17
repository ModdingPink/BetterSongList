#pragma once

#include "IFilter.hpp"

namespace BetterSongList {
    class FunctionFilter : public IFilter {
        public:
            FunctionFilter(const std::function<bool(GlobalNamespace::IPreviewBeatmapLevel*)>& func);

            virtual bool get_isReady() const override;
            virtual std::future<void> Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
        private:
            std::function<bool(GlobalNamespace::IPreviewBeatmapLevel*)> valueProvider;
    };
}