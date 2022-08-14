#pragma once

#include "IFilter.hpp"
#include "../../Utils/IAvailabilityCheck.hpp"
#include "sdc-wrapper/shared/BeatStarSong.hpp"
#include "System/Threading/Tasks/TaskCompletionSource_1.hpp"

namespace BetterSongList {
    class BasicSongDetailsFilter : public IFilter, public IAvailabilityCheck {
        public:
            BasicSongDetailsFilter(const std::function<bool(const SDC_wrapper::BeatStarSong*)>& func);

            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;

            virtual std::string GetUnavailableReason() const override;
        private:
            std::function<bool(const SDC_wrapper::BeatStarSong*)> filterValueTransformer;
            static SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> loadingTask;
    };
}