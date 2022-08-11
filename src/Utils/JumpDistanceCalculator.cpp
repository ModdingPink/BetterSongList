#include "Utils/JumpDistanceCalculator.hpp"

#include <algorithm>

namespace BetterSongList::JumpDistanceCalculator {
    float hjd(float bpm, float njs, float offset) {
		auto noteJumpMovementSpeed = (njs * bpm) / bpm;
		auto num = 60.0f / bpm;
		auto hjd = 4.0f;
		while(noteJumpMovementSpeed * num * hjd > 18.0f)
			hjd /= 2.0f;

		hjd += offset;

		return std::max(hjd, 0.25f);
    }

    float GetJd(float bpm, float njs, float offset) {
        return njs * (60.0f / bpm) * hjd(bpm, njs, offset) * 2;
    }
}