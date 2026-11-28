# Signal terminal interaction contract

## Runtime rule

A relay terminal may register objective progress only when all four conditions hold:

- The terminal has not already been activated.
- The interacting actor is a valid ALastSignalCharacter.
- The player is alive.
- The GameMode reports that the objective sequence is incomplete.

The pure C++ predicate in Source/TheLastSignal/Public/SignalInteractionRules.h keeps this eligibility decision independent of Unreal Engine. The actor calls the predicate before registering progress, so Blueprint-driven calls to the native interaction event receive the same dead-player and repeat-activation checks as the player's normal input path.

## Standalone regression test

From the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic \
  -fsanitize=undefined -fno-sanitize-recover=undefined \
  -I Source/TheLastSignal/Public \
  Tests/test_signal_interaction_rules.cpp \
  -o /tmp/test_signal_interaction_rules
/tmp/test_signal_interaction_rules
```

The 12 cases cover valid activation, repeat activation, missing player, dead player, completed objectives, and combinations of rejected conditions.

## Validation boundary

This test proves only the pure eligibility predicate. It does not execute Unreal reflection, Blueprint events, actor replication, map assets, or runtime gameplay. A real Editor playtest is still required to verify that terminals can be reached, interacted with, and counted exactly once in Relay Station 01.
