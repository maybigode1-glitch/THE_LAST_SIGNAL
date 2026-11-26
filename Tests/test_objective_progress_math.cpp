#include "ObjectiveProgressMath.h"

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <limits>

namespace
{
    void ExpectBool(const char* name, bool actual, bool expected)
    {
        if (actual != expected)
        {
            std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    void ExpectCount(const char* name, std::int32_t actual, std::int32_t expected)
    {
        if (actual != expected)
        {
            std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    void ExpectRegistration(
        const char* name,
        std::int32_t current,
        std::int32_t required,
        bool expectedAccepted,
        std::int32_t expectedUpdated
    )
    {
        std::int32_t updated = -7;
        const bool accepted = LastSignal::TryRegisterSignalCollected(current, required, updated);
        ExpectBool(name, accepted, expectedAccepted);
        ExpectCount(name, updated, expectedUpdated);
    }
}

int main()
{
    using Limits = std::numeric_limits<std::int32_t>;

    ExpectRegistration("first objective", 0, 3, true, 1);
    ExpectRegistration("middle objective", 1, 3, true, 2);
    ExpectRegistration("final objective", 2, 3, true, 3);
    ExpectRegistration("registration after completion", 3, 3, false, 3);
    ExpectRegistration("over-complete count is normalized", 9, 3, false, 3);
    ExpectRegistration("negative progress recovers from zero", -4, 3, true, 1);
    ExpectRegistration("zero target uses minimum of one", 0, 0, true, 1);
    ExpectRegistration("negative target uses minimum of one", 0, -5, true, 1);
    ExpectRegistration("maximum target increments without overflow", Limits::max() - 1, Limits::max(), true, Limits::max());
    ExpectRegistration("maximum target stops at maximum", Limits::max(), Limits::max(), false, Limits::max());
    ExpectRegistration("minimum progress recovers at maximum target", Limits::min(), Limits::max(), true, 1);
    ExpectRegistration("count above a minimum target is normalized", Limits::max(), 1, false, 1);

    ExpectBool("two of three is incomplete", LastSignal::AreSignalObjectivesComplete(2, 3), false);
    ExpectBool("three of three is complete", LastSignal::AreSignalObjectivesComplete(3, 3), true);
    ExpectBool("negative target does not complete an empty objective", LastSignal::AreSignalObjectivesComplete(0, -1), false);
    ExpectBool("over-complete count remains complete", LastSignal::AreSignalObjectivesComplete(10, 3), true);
    ExpectCount("negative count clamps to zero", LastSignal::ClampSignalsCollected(-1, 3), 0);
    ExpectCount("count clamps to target", LastSignal::ClampSignalsCollected(8, 3), 3);
    ExpectCount("required target normalizes to one", LastSignal::NormalizeRequiredSignals(0), 1);

    std::int32_t aliased = 2;
    ExpectBool("aliased output accepts final objective", LastSignal::TryRegisterSignalCollected(aliased, 3, aliased), true);
    ExpectCount("aliased output stores final objective", aliased, 3);
    ExpectBool("aliased output rejects after completion", LastSignal::TryRegisterSignalCollected(aliased, 3, aliased), false);
    ExpectCount("aliased output remains at target", aliased, 3);

    std::cout << "Objective progress rules: 22 cases passed\n";
    return EXIT_SUCCESS;
}
