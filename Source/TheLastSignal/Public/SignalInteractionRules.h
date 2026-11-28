#pragma once

namespace LastSignal
{
    // A relay terminal can be activated only once, by a valid living player,
    // while the objective sequence is still incomplete.
    constexpr bool CanActivateSignalTerminal(
        bool alreadyActivated,
        bool hasValidPlayer,
        bool playerAlive,
        bool objectivesComplete
    ) noexcept
    {
        return !alreadyActivated &&
            hasValidPlayer &&
            playerAlive &&
            !objectivesComplete;
    }
}
