#pragma once

#include <cstdint>

namespace LastSignal
{
    constexpr std::int32_t MinimumRequiredSignals = 1;

    // Enforce the editor's ClampMin contract even if a serialized or
    // programmatic value bypasses the Details-panel metadata.
    constexpr std::int32_t NormalizeRequiredSignals(std::int32_t required) noexcept
    {
        return required < MinimumRequiredSignals ? MinimumRequiredSignals : required;
    }

    // Keep the visible progress count inside [0, target].
    constexpr std::int32_t ClampSignalsCollected(
        std::int32_t collected,
        std::int32_t required
    ) noexcept
    {
        const std::int32_t target = NormalizeRequiredSignals(required);
        if (collected <= 0)
        {
            return 0;
        }
        return collected >= target ? target : collected;
    }

    constexpr bool AreSignalObjectivesComplete(
        std::int32_t collected,
        std::int32_t required
    ) noexcept
    {
        const std::int32_t target = NormalizeRequiredSignals(required);
        return ClampSignalsCollected(collected, target) >= target;
    }

    // Saturate at the target. Increment is safe even when target is INT32_MAX
    // because it occurs only while the current count is strictly below target.
    // The output may alias the caller's current-count variable.
    constexpr bool TryRegisterSignalCollected(
        std::int32_t current,
        std::int32_t required,
        std::int32_t& updated
    ) noexcept
    {
        const std::int32_t target = NormalizeRequiredSignals(required);
        updated = ClampSignalsCollected(current, target);
        if (updated >= target)
        {
            return false;
        }

        ++updated;
        return true;
    }
}
