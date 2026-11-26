#pragma once

#include <cmath>

namespace LastSignal
{
    inline bool IsValidDamageAmount(float damage) noexcept
    {
        return std::isfinite(damage) && damage > 0.0f;
    }

    // Report the health actually removed, not the requested overkill amount.
    // Invalid health/damage values leave health unchanged and report zero applied damage.
    inline bool TryApplyDamageToHealth(
        float currentHealth,
        float damage,
        float& updatedHealth,
        float& appliedDamage
    ) noexcept
    {
        updatedHealth = currentHealth;
        appliedDamage = 0.0f;
        if (!std::isfinite(currentHealth) || currentHealth <= 0.0f ||
            !IsValidDamageAmount(damage))
        {
            return false;
        }

        if (damage >= currentHealth)
        {
            updatedHealth = 0.0f;
            appliedDamage = currentHealth;
        }
        else
        {
            updatedHealth = currentHealth - damage;
            appliedDamage = damage;
        }
        return true;
    }
}
