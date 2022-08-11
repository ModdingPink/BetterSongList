#include "Utils/PPUtils.hpp"
#include <array>
#include <vector>

namespace BetterSongList::PPUtils {
    struct CurvePair {
        constexpr CurvePair(const float& item1, const float& item2) : item1(item1), item2(item2) {};
        float item1;
        float item2;
    };

    constexpr std::array<CurvePair, 32> ppCurve{{
		{0.0f, 0.0f},
		{0.6f, 0.25f},
		{0.65f, 0.29f},
		{0.7f, 0.34f},
		{0.75f, 0.40f},
		{0.8f, 0.47f},
		{0.825f, 0.51f},
		{0.85f, 0.57f},
		{0.875f, 0.655f},
		{0.9f, 0.75f},
		{0.91f, 0.79f},
		{0.92f, 0.835f},
		{0.93f, 0.885f},
		{0.94f, 0.94f},
		{0.95f, 1.0f},
		{0.955f, 1.045f},
		{0.96f, 1.11f},
		{0.965f, 1.20f},
		{0.97f, 1.31f},
		{0.9725f, 1.37f},
		{0.975f, 1.45f},
		{0.9775f, 1.57f},
		{0.98f, 1.71f},
		{0.9825f, 1.88f},
		{0.985f, 2.1f},
		{0.9875f, 2.38f},
		{0.99f, 2.73f},
		{0.9925f, 3.17f},
		{0.995f, 3.76f},
		{0.9975f, 4.7f},
		{0.999f, 5.8f},
		{1.0f, 7.0f}
    }};

    struct Slopes {
        constexpr Slopes() {
            for (int i = 0; i < slopes_values.size(); i++) {
                auto x1 = ppCurve[i].item1;
                auto y1 = ppCurve[i].item2;
                auto x2 = ppCurve[i + 1].item1;
                auto y2 = ppCurve[i + 1].item2;
                
                auto m = (y2 - y1) / (x2 - x1);
                slopes_values[i] = m;
            }
        };

        constexpr const float& operator [](int idx) const {
            return slopes_values[idx];
        }
        
        constexpr float& operator [](int idx) {
            return slopes_values[idx];
        }

        constexpr size_t size() const {
            return slopes_values.size();
        }

        std::array<float, ppCurve.size() - 1> slopes_values;
    };
    
    static constexpr Slopes slopes{};

    float Lerp(float x1, float y1, float x2, float y2, float x3, int i) {
        float m;
        if(slopes.size() < i || i < 0) {
            m = (y2 - y1) / (x2 - x1);
        } else {
            m = slopes[i];
        }

        return m * (x3 - x1) + y1;
    }

    float PPPercentage(float accuracy) {
        if (accuracy >= 1.0f) return 1.5f;
        if (accuracy <= 0.0f) return 0.0f;

        auto i = -1;

        for (const auto& [score, given] : ppCurve) {
            if (score > accuracy) break;
            i++;
        }

        auto lowerScore = ppCurve[i].item1;
        auto higherScore = ppCurve[i + 1].item1;
        auto lowerGiven = ppCurve[i].item2;
        auto higherGiven = ppCurve[i + 1].item2;
        return Lerp(lowerScore, lowerGiven, higherScore, higherGiven, accuracy, i);
    }
}