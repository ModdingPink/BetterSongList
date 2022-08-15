#include "Filters/Models/RequirementsFilter.hpp"

#include "songloader/shared/API.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"
#include "songloader/include/CustomTypes/SongLoader.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "Utils/BeatmapUtils.hpp"

namespace BetterSongList {
    bool RequirementsFilter::inited = false;

    RequirementsFilter::RequirementsFilter() 
        : IFilter() {}

    bool RequirementsFilter::get_isReady() const {
        return RuntimeSongLoader::SongLoader::GetInstance()->HasLoaded;
    }

    std::future<void> RequirementsFilter::Prepare() {
        return std::async(std::launch::async, [this](){
            auto loader = RuntimeSongLoader::SongLoader::GetInstance();
            while (!loader->HasLoaded) std::this_thread::yield();
        });
    }

    bool RequirementsFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) {
        auto h = BeatmapUtils::GetHashOfPreview(level);
        if (h.empty()) return false;

        auto customLevel = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level).value_or(nullptr);
        if (!customLevel) return false;
        auto saveData = customLevel->get_standardLevelInfoSaveData();
        if (!saveData) return false;
        auto customSaveData = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(saveData).value_or(nullptr);
        if (!customSaveData || !customSaveData->customData.has_value()) return false;

        auto& customData = customSaveData->customData.value().get();

        auto requirementsItr = customData.FindMember(u"_requirements");
        if (requirementsItr == customData.MemberEnd()) return false;
        return requirementsItr->value.Size() > 0;
    }
}