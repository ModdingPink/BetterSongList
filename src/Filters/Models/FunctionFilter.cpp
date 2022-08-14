#include "Filters/Models/FunctionFilter.hpp"

namespace BetterSongList {
    FunctionFilter::FunctionFilter(const std::function<bool(GlobalNamespace::IPreviewBeatmapLevel*)>& func) 
        : IFilter(), valueProvider(func) {

        }

    bool FunctionFilter::get_isReady() const { 
        return true;
    }

    System::Threading::Tasks::Task* FunctionFilter::Prepare() {
        return System::Threading::Tasks::Task::get_CompletedTask();
    }

    bool FunctionFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) { 
        return valueProvider(level); 
    }
}