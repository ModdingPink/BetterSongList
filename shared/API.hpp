#pragma once

#include "IFilter.hpp"
#include "ISorter.hpp"
#include "Utils/IAvailabilityCheck.hpp"
#include "Utils/ITransformerPlugin.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include <optional>

#define BETTERSONGLIST "bsl"

namespace BetterSongList::API {
    template<typename T>
    concept Filter = std::is_convertible_v<T, IFilter*>;

    template<typename T>
    concept Sorter = std::is_convertible_v<T, ISorter*>;
    
    /// @brief Register your primitive sorter for use with BSL
    /// @tparam the type of your sorter
    /// @param sorter the sorter to register
    /// @return true if successful, false if not, nullopt if BSL not installed
    template<Sorter T>
    requires(std::is_convertible_v<T, ISorterPrimitive*> && std::is_convertible_v<T, ITransformerPlugin*>)
    static std::optional<bool> RegisterPrimitiveSorter(T sorter) { 
        static auto function = CondDeps::Find<bool, ITransformerPlugin*>(BETTERSONGLIST, "RegisterPrimitiveSorter");
        if (function) 
            return function.value()(sorter);
        return std::nullopt;
    }

    /// @brief Register your custom sorter for use with BSL
    /// @tparam the type of your sorter
    /// @param sorter the sorter to register
    /// @return true if successful, false if not, nullopt if BSL not installed
    template<Sorter T>
    requires(std::is_convertible_v<T, ISorterCustom*> && std::is_convertible_v<T, ITransformerPlugin*>)
    static std::optional<bool> RegisterCustomSorter(T sorter) { 
        static auto function = CondDeps::Find<bool, ITransformerPlugin*>(BETTERSONGLIST, "RegisterCustomSorter");
        if (function) 
            return function.value()(sorter);
        return std::nullopt;
    }

    /// @brief Register your filter for use with BSL
    /// @tparam the type of your filter
    /// @param sorter the filter to register
    /// @return true if successful, false if not, nullopt if BSL not installed
    template<Filter T>
    requires(std::is_convertible_v<T, ITransformerPlugin*>)
    static std::optional<bool> RegisterFilter(T filter) { 
        static auto function = CondDeps::Find<bool, ITransformerPlugin*>(BETTERSONGLIST, "RegisterFilter");
        if (function) 
            return function.value()(filter);
        return std::nullopt;
    }
}
