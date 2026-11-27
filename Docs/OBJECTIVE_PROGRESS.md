# Objective progress contract

## Runtime contract

The GameMode owns the objective target and collected count. The shared pure C++ helper in
`Source/TheLastSignal/Public/ObjectiveProgressMath.h` keeps these values within a stable
contract even if serialized or programmatic values bypass Unreal Details-panel metadata:

- The required target is at least one.
- Collected progress is normalized to the inclusive range `0..target`.
- Registration is accepted only while progress is below the target.
- Accepted registration increments by exactly one.
- Registration at or beyond the target is rejected and progress remains saturated.
- The increment is safe when the target is `INT32_MAX`; no signed overflow is required.
- The output parameter may alias the input count.

`ALastSignalGameMode` uses the helper for registration, progress getters, required-target
reporting, and completion checks. The helper has no Unreal Engine dependency, so its
arithmetic contract can be compiled and tested independently.

## Native regression test

From the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic \
  -fsanitize=undefined -fno-sanitize-recover=undefined \
  -I Source/TheLastSignal/Public \
  Tests/test_objective_progress_math.cpp \
  -o /tmp/test_objective_progress_math
/tmp/test_objective_progress_math
```

The test covers 22 cases, including normal progress, repeated registration after
completion, invalid target/count normalization, signed 32-bit boundaries, completion
reporting, and aliased output.

## Automated validation

GitHub Actions run [#206](https://github.com/maybigode1-glitch/THE_LAST_SIGNAL/actions/runs/45824061619)
passed the repository metadata validator, the complete Python unit-test suite (including
the two new GameMode source-contract checks), and all five standalone C++ regression
executables. The new objective helper was compiled as C++17 with warnings-as-errors and
UndefinedBehaviorSanitizer; all 22 cases passed.

## Validation boundary

These checks prove only the pure objective-progress helper and the expected source-level
GameMode wiring. They do not execute Unreal reflection, Blueprint events, map assets,
Editor startup, or runtime gameplay. Those still require an authorized Unreal Editor
session and a real playtest.
