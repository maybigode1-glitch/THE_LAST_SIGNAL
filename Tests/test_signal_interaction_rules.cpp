#include "SignalInteractionRules.h"

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
    using LastSignal::CanActivateSignalTerminal;

    Expect("fresh terminal accepts a living player",
        CanActivateSignalTerminal(false, true, true, false), true);
    Expect("already activated terminal rejects repeat interaction",
        CanActivateSignalTerminal(true, true, true, false), false);
    Expect("missing player is rejected",
        CanActivateSignalTerminal(false, false, false, false), false);
    Expect("dead player is rejected",
        CanActivateSignalTerminal(false, true, false, false), false);
    Expect("completed objective sequence rejects activation",
        CanActivateSignalTerminal(false, true, true, true), false);
    Expect("missing player cannot pass with alive flag set",
        CanActivateSignalTerminal(false, false, true, false), false);
    Expect("dead player cannot pass after a previous activation",
        CanActivateSignalTerminal(true, true, false, false), false);
    Expect("completed sequence rejects an already activated terminal",
        CanActivateSignalTerminal(true, true, true, true), false);
    Expect("all invalid conditions reject activation",
        CanActivateSignalTerminal(true, false, false, true), false);
    Expect("unactivated terminal still rejects without a player",
        CanActivateSignalTerminal(false, false, true, true), false);
    Expect("living player is rejected when objectives are complete",
        CanActivateSignalTerminal(false, true, true, true), false);
    Expect("living player can activate a different fresh terminal",
        CanActivateSignalTerminal(false, true, true, false), true);

    std::cout << "Signal interaction rules: " << CasesRun << " cases passed\n";
    return EXIT_SUCCESS;
}
