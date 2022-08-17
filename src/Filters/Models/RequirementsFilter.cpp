#include "Filters/Models/RequirementsFilter.hpp"
#include "logging.hpp"

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
        auto customLevel = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level).value_or(nullptr);
        if (!customLevel) {
            DEBUG("Level was not custom level!");
            return false;
        }
        auto saveData = customLevel->get_standardLevelInfoSaveData();
        if (!saveData) {
            DEBUG("Level had no save data!");
            return false;
        }

        auto customSaveData = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(saveData).value_or(nullptr);
        if (!customSaveData) {

            DEBUG("Could not get custom save data!");
            return false;
        }

        if (customSaveData->doc.use_count() <= 0) {
            DEBUG("Document had use count of 0!");
            return false;
        }

        // :smilew:
        auto& doc = *customSaveData->doc.get();
        auto difficultyBeatmapSetsitr = doc.FindMember(u"_difficultyBeatmapSets");
        if (difficultyBeatmapSetsitr != doc.MemberEnd()) {
            auto setArr = difficultyBeatmapSetsitr->value.GetArray();
            for (auto& beatmapCharacteristicItr : setArr) {
                auto difficultyBeatmaps = beatmapCharacteristicItr.FindMember(u"_difficultyBeatmaps");
                auto beatmaps = difficultyBeatmaps->value.GetArray();
                for (auto& beatmap : beatmaps) {
                    auto customDataItr = beatmap.FindMember(u"_customData");
                    if (customDataItr != beatmap.MemberEnd()) {
                        auto& customData = customDataItr->value;
                        auto requirementsItr = customData.FindMember(u"_requirements");
                        if (requirementsItr != customData.MemberEnd()) {
                            if (requirementsItr->value.Size() > 0) return true;
                        }
                    }
                }
            }
        }
        
        DEBUG("Custom Data contained 0 requirements!");
        return false;
    }
}