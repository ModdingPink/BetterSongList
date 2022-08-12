#include "Filters/Models/FunctionFilter.hpp"

namespace BetterSongList {
    FunctionFilter::FunctionFilter(const std::function<bool(GlobalNamespace::IPreviewBeatmapLevel*)>& func) 
        : IFilter(), valueProvider(func) {

        }

    bool FunctionFilter::get_isReady() const { 
        return true; 
    }

    void FunctionFilter::Prepare() {

    }

    bool FunctionFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        return valueProvider(level); 
    }
}