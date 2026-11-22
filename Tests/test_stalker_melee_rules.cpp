#include "StalkerMeleeRules.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace
{
    void Expect(const char* name, bool actual, bool expected)
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
    using LastSignal::CanApplyStalkerMeleeDamage;

    Expect("visible player inside attack range", CanApplyStalkerMeleeDamage(120.0f, 150.0f, true, true), true);
    Expect("visible player at grace boundary", CanApplyStalkerMeleeDamage(225.0f, 150.0f, true, true), true);
    Expect("player beyond grace boundary", CanApplyStalkerMeleeDamage(225.01f, 150.0f, true, true), false);
    Expect("missing AI controller cancels impact", CanApplyStalkerMeleeDamage(120.0f, 150.0f, false, true), false);
    Expect("blocked line of sight cancels impact", CanApplyStalkerMeleeDamage(120.0f, 150.0f, true, false), false);
    Expect("missing controller and line of sight cancels impact", CanApplyStalkerMeleeDamage(120.0f, 150.0f, false, false), false);
    Expect("NaN distance cancels impact", CanApplyStalkerMeleeDamage(std::numeric_limits<float>::quiet_NaN(), 150.0f, true, true), false);
    Expect("NaN attack range cancels impact", CanApplyStalkerMeleeDamage(120.0f, std::numeric_limits<float>::quiet_NaN(), true, true), false);

    std::cout << "Stalker melee impact rules: 8 cases passed\n";
    return EXIT_SUCCESS;
}
