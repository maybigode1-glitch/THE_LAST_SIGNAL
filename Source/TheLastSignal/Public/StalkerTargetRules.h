#pragma once

namespace LastSignal
{
    // The stalker may pursue or attack only while AI possession and a living
    // player target are both available. Otherwise the caller must fail closed.
    constexpr bool CanUpdateStalkerBehaviour(
        bool hasAIController,
        bool hasLivingPlayer
    ) noexcept
    {
        return hasAIController && hasLivingPlayer;
    }
}
