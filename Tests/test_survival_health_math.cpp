#include "SurvivalHealthMath.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace
{
    void ExpectApply(
        const char* name,
        float currentHealth,
        float damage,
        bool expectedAccepted,
        float expectedHealth
    )
    {
        float updatedHealth = currentHealth;
        const bool accepted = LastSignal::TryApplyDamageToHealth(
            currentHealth, damage, updatedHealth
        );
        const bool healthMatches = updatedHealth == expectedHealth ||
            (std::isnan(updatedHealth) && std::isnan(expectedHealth));
        if (accepted != expectedAccepted || !healthMatches)
        {
            std::cerr << name << ": expected accepted=" << expectedAccepted
                      << ", health=" << expectedHealth << "; got accepted="
                      << accepted << ", health=" << updatedHealth << '\n';
            std::exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    using Limits = std::numeric_limits<float>;

    ExpectApply("ordinary damage", 100.0f, 15.0f, true, 85.0f);
    ExpectApply("fractional damage", 100.0f, 0.25f, true, 99.75f);
    ExpectApply("exact lethal damage", 100.0f, 100.0f, true, 0.0f);
    ExpectApply("overkill clamps to zero", 100.0f, 125.0f, true, 0.0f);
    ExpectApply("zero damage is rejected", 100.0f, 0.0f, false, 100.0f);
    ExpectApply("negative damage is rejected", 100.0f, -1.0f, false, 100.0f);
    ExpectApply("NaN damage is rejected", 100.0f, Limits::quiet_NaN(), false, 100.0f);
    ExpectApply("positive infinite damage is rejected", 100.0f, Limits::infinity(), false, 100.0f);
    ExpectApply("negative infinite damage is rejected", 100.0f, -Limits::infinity(), false, 100.0f);
    ExpectApply("NaN current health is rejected", Limits::quiet_NaN(), 1.0f, false, Limits::quiet_NaN());
    ExpectApply("infinite current health is rejected", Limits::infinity(), 1.0f, false, Limits::infinity());
    ExpectApply("zero current health is rejected", 0.0f, 1.0f, false, 0.0f);
    ExpectApply("negative current health is rejected", -1.0f, 1.0f, false, -1.0f);

    std::cout << "Survival health damage rules: 13 cases passed\n";
    return EXIT_SUCCESS;
}
