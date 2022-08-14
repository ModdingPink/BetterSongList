#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "System/Threading/Tasks/Task.hpp"

namespace BetterSongList {
    class IFilter {
        public:
            /// @brief cast the sorter to a derived type
            /// @tparam T the derived type to cast to
            /// @return this as derived type or nullptr if invalid
            template<typename T>
            constexpr T as() const { 
                return dynamic_cast<T>(this);
            }

            /// @brief cast the sorter to a derived type
            /// @tparam T the derived type to cast to
            /// @return this as derived type or nullptr if invalid
            template<typename T>
            constexpr T as() { 
                return dynamic_cast<T>(this);
            }

            virtual bool get_isReady() const = 0;
            virtual System::Threading::Tasks::Task* Prepare() = 0;
            virtual bool GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) = 0;
    };
}