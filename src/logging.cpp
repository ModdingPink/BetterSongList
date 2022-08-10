#include "logging.hpp"
#include <map>
#include <string>

std::map<std::string, LoggerContextObject> contextLoggers;
namespace BetterSongList
{
    Logger& Logging::getLogger()
    {
        static Logger* logger = new Logger({MOD_ID, VERSION}, LoggerOptions(false, true));
        return *logger;
    }
}