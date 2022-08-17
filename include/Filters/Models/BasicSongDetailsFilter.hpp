#pragma once

#include "IFilter.hpp"
#include "Utils/IAvailabilityCheck.hpp"
#include "sdc-wrapper/shared/BeatStarSong.hpp"

namespace BetterSongList {
    class BasicSongDetailsFilter : public IFilter, public IAvailabilityCheck {
        public:
            BasicSongDetailsFilter(const std::function<bool(const SDC_wrapper::BeatStarSong*)>& func);

            virtual bool get_isReady() const override;
            virtual std::future<void> Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;

            virtual std::string GetUnavailableReason() const override;
        private:
            std::function<bool(const SDC_wrapper::BeatStarSong*)> filterValueTransformer;
    };
}