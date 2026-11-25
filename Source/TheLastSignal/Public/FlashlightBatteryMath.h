#pragma once

#include <cmath>

namespace LastSignal
{
    constexpr float MaxFlashlightBattery = 100.0f;

    // Invalid stored values fail closed to an empty battery.
    inline float ClampFlashlightBattery(float battery) noexcept
    {
        if (!std::isfinite(battery) || battery <= 0.0f)
        {
            return 0.0f;
        }
        return battery >= MaxFlashlightBattery ? MaxFlashlightBattery : battery;
    }

    // Ignore invalid time/rate inputs and use double for the product so finite
    // float inputs cannot overflow before the remaining charge is clamped.
    inline float DrainFlashlightBattery(
        float currentBattery,
        float drainPerSecond,
        float deltaSeconds
    ) noexcept
    {
        const float normalizedBattery = ClampFlashlightBattery(currentBattery);
        if (!std::isfinite(currentBattery) ||
            !std::isfinite(drainPerSecond) ||
            !std::isfinite(deltaSeconds) ||
            drainPerSecond <= 0.0f ||
            deltaSeconds <= 0.0f)
        {
            return normalizedBattery;
        }

        const double remaining =
            static_cast<double>(normalizedBattery) -
            static_cast<double>(drainPerSecond) * static_cast<double>(deltaSeconds);
        if (remaining <= 0.0)
        {
            return 0.0f;
        }
        return remaining >= MaxFlashlightBattery
            ? MaxFlashlightBattery
            : static_cast<float>(remaining);
    }

    // A pack is consumed only when a finite positive charge actually increases
    // the normalized battery value. The output may alias currentBattery.
    inline bool TryRechargeFlashlightBattery(
        float currentBattery,
        float charge,
        float& updatedBattery
    ) noexcept
    {
        // Snapshot inputs before writing the output so aliasing is well-defined.
        const bool hasFiniteCurrentBattery = std::isfinite(currentBattery);
        const float normalizedBattery = ClampFlashlightBattery(currentBattery);
        updatedBattery = normalizedBattery;

        if (!hasFiniteCurrentBattery ||
            !std::isfinite(charge) ||
            charge <= 0.0f ||
            normalizedBattery >= MaxFlashlightBattery)
        {
            return false;
        }

        const float room = MaxFlashlightBattery - normalizedBattery;
        updatedBattery = charge >= room
            ? MaxFlashlightBattery
            : normalizedBattery + charge;
        return updatedBattery > normalizedBattery;
    }
}
