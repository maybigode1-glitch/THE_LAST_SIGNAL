#include "BatteryInventoryMath.h"

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <limits>

namespace
{
    void Expect(const char* name, std::int32_t actual, std::int32_t expected)
    {
        if (actual != expected)
        {
            std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    void ExpectTryAdd(
        const char* name,
        std::int32_t current,
        std::int32_t amount,
        bool expectedAccepted,
        std::int32_t expectedCount
    )
    {
        std::int32_t updated = current;
        const bool accepted = LastSignal::TryAddBatteryPacks(current, amount, updated);
        if (accepted != expectedAccepted)
        {
            std::cerr << name << ": expected accepted=" << expectedAccepted
                      << ", got " << accepted << '\n';
            std::exit(EXIT_FAILURE);
        }
        Expect(name, updated, expectedCount);
    }
}

int main()
{
    Expect("first pickup", LastSignal::AddBatteryPacks(0, 1), 1);
    Expect("ordinary addition", LastSignal::AddBatteryPacks(4, 3), 7);
    Expect("reaches cap", LastSignal::AddBatteryPacks(98, 1), 99);
    Expect("addition at cap", LastSignal::AddBatteryPacks(99, 1), 99);
    Expect("large addition at cap", LastSignal::AddBatteryPacks(80, 50), 99);
    Expect("zero amount is a no-op", LastSignal::AddBatteryPacks(12, 0), 12);
    Expect("negative amount is a no-op", LastSignal::AddBatteryPacks(12, -5), 12);
    Expect("INT32_MAX amount is safe", LastSignal::AddBatteryPacks(1, std::numeric_limits<std::int32_t>::max()), 99);
    Expect("large current plus positive amount is safe", LastSignal::AddBatteryPacks(std::numeric_limits<std::int32_t>::max(), 1), 99);
    Expect("negative intermediate is clamped", LastSignal::AddBatteryPacks(std::numeric_limits<std::int32_t>::min(), 1), 0);

    ExpectTryAdd("pickup accepted into empty inventory", 0, 1, true, 1);
    ExpectTryAdd("pickup partially accepted near cap", 98, 5, true, 99);
    ExpectTryAdd("pickup rejected at cap", 99, 1, false, 99);
    ExpectTryAdd("large pickup rejected at cap", 99, 50, false, 99);
    ExpectTryAdd("zero-value pickup rejected", 12, 0, false, 12);
    ExpectTryAdd("negative-value pickup rejected", 12, -1, false, 12);

    std::cout << "Battery inventory arithmetic and pickup acceptance: 16 cases passed\n";
    return EXIT_SUCCESS;
}
