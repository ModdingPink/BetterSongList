#include "modloader/shared/modloader.hpp"
#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "assets.hpp"
#include "config.hpp"

#include "Utils/SongDetails.hpp"

ModInfo modInfo{MOD_ID, VERSION};

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    Hooks::InstallHooks(BetterSongList::Logging::getLogger());
    custom_types::Register::AutoRegister();

    if (!LoadConfig()) SaveConfig();

    BetterSongList::SongDetails::Init();
}

BSML_DATACACHE(double_arrow) {
    return IncludedAssets::DoubleArrowIcon_png;
}

BSML_DATACACHE(carat_up) {
    return IncludedAssets::CaratUp_png;
}

BSML_DATACACHE(carat_down) {
    return IncludedAssets::CaratDown_png;
}
