#pragma once

#include "ISorter.hpp"

namespace BetterSongList {
    class PrimitiveFunctionSorter : public ISorterPrimitive {
        public:
            using ValueGetterFunc = std::function<std::optional<float>(GlobalNamespace::IPreviewBeatmapLevel*)>;
            PrimitiveFunctionSorter(const ValueGetterFunc& sortFunc);
            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual std::optional<float> GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const override;
        private:
            ValueGetterFunc sortValueGetter;
    };

    class PrimitiveFunctionSorterWithLegend : public PrimitiveFunctionSorter, public ISorterWithLegend {
        public:
            using LegendGetterFunc = std::function<std::string(GlobalNamespace::IPreviewBeatmapLevel*)>;
            PrimitiveFunctionSorterWithLegend(const ValueGetterFunc& sortFunc, const LegendGetterFunc& legendFunc);
            virtual ISorterWithLegend::Legend BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const override;
        private:
            LegendGetterFunc legendValueGetter;
    };

    class ComparableFunctionSorter : public ISorterCustom {
        public:
            using CompareFunc = std::function<int(GlobalNamespace::IPreviewBeatmapLevel*, GlobalNamespace::IPreviewBeatmapLevel*)>;
            ComparableFunctionSorter(const CompareFunc& sortFunc);
            virtual bool get_isReady() const override;
            virtual System::Threading::Tasks::Task* Prepare() override;
            virtual void DoSort(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& levels, bool ascending) const override;
            int operator ()(GlobalNamespace::IPreviewBeatmapLevel* lhs, GlobalNamespace::IPreviewBeatmapLevel* rhs) const;
        private:
            CompareFunc sortValueGetter;
    };

    class ComparableFunctionSorterWithLegend : public ComparableFunctionSorter, public ISorterWithLegend {
        public:
            using LegendGetterFunc = std::function<std::string(GlobalNamespace::IPreviewBeatmapLevel*)>;
            ComparableFunctionSorterWithLegend(const ComparableFunctionSorter::CompareFunc& sortFunc, const LegendGetterFunc& legendFunc);
            virtual ISorterWithLegend::Legend BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const override;
        private:
            LegendGetterFunc legendValueGetter;
    };
}