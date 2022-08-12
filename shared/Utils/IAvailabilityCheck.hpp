#pragma once

#include <string>

namespace BetterSongList {
    class IAvailabilityCheck {
        public:
            virtual std::string GetUnavailableReason() const = 0;
    };
}