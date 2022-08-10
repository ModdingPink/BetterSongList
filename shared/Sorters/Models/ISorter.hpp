#pragma once

#include <map>
#include <optional>
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

namespace BetterSongList {
    namespace Hooks {
        class HookLevelCollectionTableSet;
    }

    enum class SorterType {
        None = 0,
        WithLegend = 1,
        Custom = 2,
        Primitive = 4
    };

    static SorterType operator &(const SorterType& a, const SorterType& b) {
        return static_cast<SorterType>(static_cast<int>(a) & static_cast<int>(b));
    }

    static SorterType& operator &=(SorterType& a, const SorterType& b) {
        a = a & b;
        return a;
    }

    static SorterType operator |(const SorterType& a, const SorterType& b) {
        return static_cast<SorterType>(static_cast<int>(a) | static_cast<int>(b));
    }

    static SorterType& operator |=(SorterType& a, const SorterType& b) {
        a = a | b;
        return a;
    }

    class ISorter {
        protected:
            friend class ::BetterSongList::Hooks::HookLevelCollectionTableSet;
            SorterType type;
        public:
            ISorter(SorterType type = SorterType::None) : type(type) {}
            virtual bool get_isReady() const = 0;
            virtual void Prepare() = 0;
    };

    class ISorterWithLegend : public ISorter {
        public:
            ISorterWithLegend() : ISorter() { this->type |= SorterType::WithLegend; }
            virtual std::map<std::string, int> BuildLegend(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels) const = 0;
    };

    class ISorterCustom : public ISorter {
        public:
            ISorterCustom() : ISorter() { this->type |= SorterType::Custom; }
            virtual void DoSort(ArrayW<GlobalNamespace::IPreviewBeatmapLevel*> levels, bool ascending) const = 0;
    };

    class ISorterPrimitive : public ISorter {
        public:
            ISorterPrimitive() : ISorter() { this->type |= SorterType::Primitive; }
            virtual std::optional<float> GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) const = 0;
    };
}