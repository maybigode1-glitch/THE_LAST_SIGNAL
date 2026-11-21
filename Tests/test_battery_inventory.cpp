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

    std::cout << "Battery inventory arithmetic: 10 cases passed\n";
    return EXIT_SUCCESS;
}
