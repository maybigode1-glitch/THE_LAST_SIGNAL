#pragma once

#include <cmath>

namespace LastSignal
{
    inline bool IsValidDamageAmount(float damage) noexcept
    {
        return std::isfinite(damage) && damage > 0.0f;
    }

    // Reject invalid health/damage values before they can poison the player's health state.
    inline bool TryApplyDamageToHealth(
        float currentHealth,
        float damage,
        float& updatedHealth
    ) noexcept
    {
        updatedHealth = currentHealth;
        if (!std::isfinite(currentHealth) || currentHealth <= 0.0f ||
            !IsValidDamageAmount(damage))
        {
            return false;
        }

        updatedHealth = damage >= currentHealth ? 0.0f : currentHealth - damage;
        return true;
    }
}
