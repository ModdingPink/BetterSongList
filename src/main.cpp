#include "modloader/shared/modloader.hpp"
#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/register.hpp"

ModInfo modInfo{MOD_ID, VERSION};

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    Hooks::InstallHooks(BetterSongList::Logging::getLogger());
    custom_types::Register::AutoRegister();
}