#pragma once

#include <cmath>

namespace LastSignal
{
    constexpr float StalkerMeleeRangeGrace = 75.0f;

    inline bool CanApplyStalkerMeleeDamage(
        float distance,
        float attackRange,
        bool hasAIController,
        bool hasLineOfSight
    ) noexcept
    {
        if (!hasAIController || !hasLineOfSight ||
            !std::isfinite(distance) || !std::isfinite(attackRange) ||
            distance < 0.0f || attackRange < 0.0f)
        {
            return false;
        }

        return distance <= attackRange + StalkerMeleeRangeGrace;
    }
}
