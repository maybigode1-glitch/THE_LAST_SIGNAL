#pragma once

#include <cstdint>

namespace LastSignal
{
    constexpr std::int32_t MaxBatteryPacks = 99;

    // Keep the inventory addition defined even when Blueprint supplies INT32_MAX.
    constexpr std::int32_t AddBatteryPacks(std::int32_t current, std::int32_t amount) noexcept
    {
        if (amount <= 0)
        {
            return current;
        }

        const std::int64_t total =
            static_cast<std::int64_t>(current) + static_cast<std::int64_t>(amount);

        if (total <= 0)
        {
            return 0;
        }
        if (total >= MaxBatteryPacks)
        {
            return MaxBatteryPacks;
        }
        return static_cast<std::int32_t>(total);
    }

    // Returns true only when the inventory count increases. Pickups must not be
    // consumed when the inventory is already at its cap.
    constexpr bool TryAddBatteryPacks(
        std::int32_t current,
        std::int32_t amount,
        std::int32_t& updated
    ) noexcept
    {
        updated = AddBatteryPacks(current, amount);
        return updated > current;
    }
}
