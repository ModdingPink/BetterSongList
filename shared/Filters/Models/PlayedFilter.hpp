#pragma once

#include "IFilter.hpp"
#include "System/Threading/Tasks/TaskCompletionSource_1.hpp"

namespace BetterSongList {
    class PlayedFilter : public IFilter {
        public:
            PlayedFilter(bool unplayed = false);

            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
        private:
            bool intendedPlayedState;
            static SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> loadingTask;
    };
}