#include "config.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "logging.hpp"

Config config;

Configuration& get_config() {
    static Configuration config({MOD_ID, VERSION});
    config.Load();
    return config;
}

#define Save(identifier) doc.AddMember(#identifier, config.identifier, allocator)

void SaveConfig() {
    INFO("Saving Configuration...");
    rapidjson::Document& doc = get_config().config;

    doc.RemoveAllMembers();
    doc.SetObject();

    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    Save(lastSong);
    Save(lastPack);
    Save(lastSort);
    Save(lastFilter);
    doc.AddMember("lastCategory", config.lastCategory.value, allocator);
    Save(reselectLastSong);
    Save(sortAscending);
    Save(enableAlphabetScrollBar);
    Save(clearFiltersOnPlaylistSelect);
    Save(modBasegameSearch);
    Save(autoFilterUnowned);

    get_config().Write();
    INFO("Saved Configuration!");
}

#define GET_INT(identifier)                                   \
    auto identifier##_itr = doc.FindMember(#identifier);      \
    if (identifier##_itr != doc.MemberEnd())                  \
        config.identifier = identifier##_itr->value.GetInt(); \
    else                                                      \
        foundEverything = false;                              \
    
#define GET_STRING(identifier)                                      \
    auto identifier##_itr = doc.FindMember(#identifier);            \
    if (identifier##_itr != doc.MemberEnd())                        \
        config.identifier = identifier##_itr->value.GetString();    \
    else                                                            \
        foundEverything = false;                                    \

#define GET_BOOL(identifier)                                   \
    auto identifier##_itr = doc.FindMember(#identifier);       \
    if (identifier##_itr != doc.MemberEnd())                   \
        config.identifier = identifier##_itr->value.GetBool(); \
    else                                                       \
        foundEverything = false;                               \


bool LoadConfig() {
    INFO("Loading Configuration...");
    bool foundEverything = true;
    rapidjson::Document& doc = get_config().config;

    GET_STRING(lastSong);
    GET_STRING(lastPack);
    GET_STRING(lastSort);
    GET_STRING(lastFilter);
    GET_INT(lastCategory);
    GET_BOOL(reselectLastSong);
    GET_BOOL(sortAscending);
    GET_BOOL(enableAlphabetScrollBar);
    GET_BOOL(clearFiltersOnPlaylistSelect);
    GET_BOOL(modBasegameSearch);
    GET_BOOL(autoFilterUnowned);

    if (foundEverything)
        INFO("Loaded Configuration!");
    return foundEverything;
}