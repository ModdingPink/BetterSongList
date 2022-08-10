#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

namespace BetterSongList {
    class IFilter {
        public:
            virtual bool get_isReady() const = 0;
            virtual void Prepare() = 0;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) = 0;
    };
}