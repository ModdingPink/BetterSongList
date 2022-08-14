#pragma once

#include <unordered_map>
#include <optional>
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "System/Threading/Tasks/Task.hpp"

namespace BetterSongList {
    namespace Hooks {
        class HookLevelCollectionTableSet;
    }

    class ISorter {
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

            constexpr ISorter() {}
            virtual bool get_isReady() const = 0;
            virtual System::Threading::Tasks::Task* Prepare() = 0;
        protected:
            friend class ::BetterSongList::Hooks::HookLevelCollectionTableSet;
    };

    class ISorterWithLegend {
        public:
            struct LegendPair {
                LegendPair(const std::string& str, const int& num) : first(str), second(num) {}
                std::string first;
                int second;
            };
            
            using Legend = std::vector<LegendPair>;
            constexpr ISorterWithLegend() {};
            virtual Legend BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const = 0;
    };

    class ISorterCustom : public ISorter {
        public:
            constexpr ISorterCustom() : ISorter() {}
            virtual void DoSort(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*>& levels, bool ascending) const = 0;
    };

    class ISorterPrimitive : public ISorter {
        public:
            constexpr ISorterPrimitive() : ISorter() {}
            virtual std::optional<float> GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const = 0;
    };
}