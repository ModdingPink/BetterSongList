#pragma once

#include "IFilter.hpp"
#include "Utils/ITransformerPlugin.hpp"
#include <map>

namespace BetterSongList {

    template<typename T>
    concept Filter = std::is_convertible_v<T, IFilter*>;

    class FilterMethods {
        public:
            static const std::map<std::string, IFilter*>& get_methods();

            template<Filter T>
            requires(std::is_convertible_v<T, ITransformerPlugin*>)
            static bool RegisterFilter(T filter) { return Register(filter); }

        private:
            static bool Register(ITransformerPlugin* filter);
            static std::map<std::string, IFilter*> methods;
    };
}