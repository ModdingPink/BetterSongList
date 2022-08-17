#pragma once

#include <optional>
#include "GlobalNamespace/LevelCollectionTableView.hpp"

namespace BetterSongList::Hooks {
    class RestoreTableScroll {
        public:
            /// @brief prio int max value
            static void LevelCollectionTableView_Init_Prefix(GlobalNamespace::LevelCollectionTableView* self);
            /// @brief prio int min value
            static void LevelCollectionTableView_SetData_PostFix(GlobalNamespace::LevelCollectionTableView* self);
            static void ResetScroll();
        private:
            friend class DoTheFunnySelect;
            static std::optional<int> scrollToIndex;
            static bool doResetScrollOnNext;
            static bool gotoLastSelectedOnNextSetData;
    };
}