#include "StalkerTargetRules.h"

#include <cstdlib>
#include <iostream>

namespace
{
    int CasesRun = 0;

    void Expect(const char* name, bool actual, bool expected)
    {
        ++CasesRun;
        if (actual != expected)
        {
            std::cerr << name << ": expected " << expected
                      << ", got " << actual << '\n';
            std::exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    using LastSignal::CanUpdateStalkerBehaviour;

    static_assert(CanUpdateStalkerBehaviour(true, true), "valid target should permit behaviour updates");
    static_assert(!CanUpdateStalkerBehaviour(false, true), "lost AI possession must fail closed");
    static_assert(!CanUpdateStalkerBehaviour(true, false), "missing or dead player must fail closed");
    static_assert(!CanUpdateStalkerBehaviour(false, false), "missing controller and player must fail closed");

    Expect("AI controller and living player allow behaviour update",
        CanUpdateStalkerBehaviour(true, true), true);
    Expect("lost AI possession requires shutdown",
        CanUpdateStalkerBehaviour(false, true), false);
    Expect("missing or dead player requires shutdown",
        CanUpdateStalkerBehaviour(true, false), false);
    Expect("missing controller and player require shutdown",
        CanUpdateStalkerBehaviour(false, false), false);

    std::cout << "Stalker target rules: " << CasesRun << " cases passed" << '\n';
    return EXIT_SUCCESS;
}
