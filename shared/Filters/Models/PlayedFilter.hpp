#pragma once

#include "IFilter.hpp"

namespace BetterSongList {
    class PlayedFilter : public IFilter {
        public:
            PlayedFilter(bool unplayed = false);

            virtual bool get_isReady() const override;
            virtual void Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
        private:
            bool intendedPlayedState;
    };
}