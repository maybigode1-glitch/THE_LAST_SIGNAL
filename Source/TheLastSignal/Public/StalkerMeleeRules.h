#pragma once

namespace LastSignal
{
    constexpr float StalkerMeleeRangeGrace = 75.0f;

    constexpr bool CanApplyStalkerMeleeDamage(
        float distance,
        float attackRange,
        bool hasAIController,
        bool hasLineOfSight
    ) noexcept
    {
        return hasAIController &&
               hasLineOfSight &&
               distance <= attackRange + StalkerMeleeRangeGrace;
    }
}
