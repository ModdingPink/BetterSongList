#pragma once

#include <optional>
#include "GlobalNamespace/LevelCollectionTableView.hpp"

namespace BetterSongList::Hooks {
    class RestoreTableScroll {
        public:
            static void Prefix(GlobalNamespace::LevelCollectionTableView* self);
            static void ResetScroll();
        private:
            friend class DoTheFunnySelect;
            static std::optional<int> scrollToIndex;
            static bool doResetScrollOnNext;
            static bool gotoLastSelectedOnNextSetData;
    };

    class DoTheFunnySelect {
        public:
            static void PostFix(GlobalNamespace::LevelCollectionTableView* self);
    };
}