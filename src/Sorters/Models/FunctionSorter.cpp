#include "Sorters/Models/FunctionSorter.hpp"

#include "Utils/SongListLegendBuilder.hpp"

namespace BetterSongList {
    /* PrimitiveFunctionSorter */
    PrimitiveFunctionSorter::PrimitiveFunctionSorter(
        const PrimitiveFunctionSorter::ValueGetterFunc& sortFunc
    ) : 
        ISorterPrimitive(), 
        sortValueGetter(sortFunc) {}

    bool PrimitiveFunctionSorter::get_isReady() const {
        return true;
    }

    void PrimitiveFunctionSorter::Prepare() {}

    std::optional<float> PrimitiveFunctionSorter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const {
        return sortValueGetter(level);
    }
    /* PrimitiveFunctionSorter */

    /* PrimitiveFunctionSorterWithLegend */
    PrimitiveFunctionSorterWithLegend::PrimitiveFunctionSorterWithLegend(
        const PrimitiveFunctionSorter::ValueGetterFunc& sortFunc, 
        const PrimitiveFunctionSorterWithLegend::LegendGetterFunc& legendFunc
    ) : 
        PrimitiveFunctionSorter(sortFunc), 
        ISorterWithLegend(), 
        legendValueGetter(legendFunc) {}
    
    ISorterWithLegend::Legend PrimitiveFunctionSorterWithLegend::BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const {
		return SongListLegendBuilder::BuildFor(levels, legendValueGetter);
    }

    /* PrimitiveFunctionSorterWithLegend */
    /* ComparableFunctionSorter */

    ComparableFunctionSorter::ComparableFunctionSorter(
        const CompareFunc& sortFunc
    ) : 
        ISorterCustom(), 
        sortValueGetter(sortFunc) {}

    bool ComparableFunctionSorter::get_isReady() const {
        return true;
    }

    void ComparableFunctionSorter::Prepare() {}

    void ComparableFunctionSorter::DoSort(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& levels, bool ascending) const {
        std::sort(levels.begin(), levels.end(), *this);
    }

    int ComparableFunctionSorter::operator ()(GlobalNamespace::IPreviewBeatmapLevel* lhs, GlobalNamespace::IPreviewBeatmapLevel* rhs) const {
        return sortValueGetter(lhs, rhs);
    }
    /* ComparableFunctionSorter */
    /* ComparableFunctionSorterWithLegend */
    
    ComparableFunctionSorterWithLegend::ComparableFunctionSorterWithLegend(
        const ComparableFunctionSorter::CompareFunc& sortFunc, 
        const ComparableFunctionSorterWithLegend::LegendGetterFunc& legendFunc
    ) :
        ComparableFunctionSorter(sortFunc),
        ISorterWithLegend(),
        legendValueGetter(legendFunc) {

    }

    ISorterWithLegend::Legend ComparableFunctionSorterWithLegend::BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const {
		return SongListLegendBuilder::BuildFor(levels, legendValueGetter);
    }
    /* ComparableFunctionSorterWithLegend */
}