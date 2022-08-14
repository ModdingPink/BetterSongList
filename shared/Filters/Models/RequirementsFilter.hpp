#pragma once

#include "IFilter.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include <vector>
#include "System/Threading/Tasks/TaskCompletionSource_1.hpp"

namespace BetterSongList {
    class RequirementsFilter : public IFilter {
        public:
            RequirementsFilter();

            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) override;
            void SongsLoadedCallback(const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs);
        private:
            static bool inited;
            static SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> loadingTask;
    };
}