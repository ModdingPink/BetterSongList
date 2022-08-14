#pragma once

#include "ISorter.hpp"
#include "../../Utils/IAvailabilityCheck.hpp"
#include "sdc-wrapper/shared/BeatStarSong.hpp"
#include "System/Threading/Tasks/TaskCompletionSource_1.hpp"

namespace BetterSongList {
    class BasicSongDetailsSorterWithLegend : public ISorterWithLegend, public ISorterPrimitive, public IAvailabilityCheck {
        public:
            using ValueGetterFunc = std::function<std::optional<float>(const SDC_wrapper::BeatStarSong*)>;
            using LegendGetterFunc = std::function<std::string(const SDC_wrapper::BeatStarSong*)>;
            BasicSongDetailsSorterWithLegend(ValueGetterFunc sortFunc);
            BasicSongDetailsSorterWithLegend(ValueGetterFunc sortFunc, LegendGetterFunc legendFunc);
            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual Legend BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const override;
            virtual std::optional<float> GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const override;
            virtual std::string GetUnavailableReason() const override;
            std::string DefaultLegendGetter(const SDC_wrapper::BeatStarSong* song) const;
        private:
            ValueGetterFunc sortValueGetter;
            LegendGetterFunc legendValueGetter;
            static SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> loadingTask;
    };
}