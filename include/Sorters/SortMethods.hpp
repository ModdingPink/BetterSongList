#pragma once

#include "ISorter.hpp"
#include "Utils/ITransformerPlugin.hpp"
#include <map>

namespace BetterSongList {

    template<typename T>
    concept Sorter = std::is_convertible_v<T, ISorter*>;

    class SortMethods {
        public:
            static const std::map<std::string, ISorter*>& get_methods();
            static bool Register(ITransformerPlugin* sorter);

            template<Sorter T>
            requires(std::is_convertible_v<T, ISorterPrimitive*> && std::is_convertible_v<T, ITransformerPlugin*>)
            static bool RegisterPrimitiveSorter(T sorter) { return Register(sorter); }

            template<Sorter T>
            requires(std::is_convertible_v<T, ISorterCustom*> && std::is_convertible_v<T, ITransformerPlugin*>)
            static bool RegisterCustomSorter(T sorter) { return Register(sorter); }
        private:
            static std::map<std::string, ISorter*> methods;
    };
}