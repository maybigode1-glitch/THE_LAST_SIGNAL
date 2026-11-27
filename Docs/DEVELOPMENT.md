# Development and test environment

## Repository CI lane

GitHub Actions uses a GitHub-hosted Ubuntu runner for checks that do not require Unreal Engine. The current workflow runs Python 3.12 and the system GCC C++ compiler.

## What CI verifies

- `Scripts/validate_project.py` checks the project descriptor, required repository files, and basic Unreal project metadata.
- `python -m unittest discover -s Tests -v` runs the complete Python regression suite. This includes project metadata checks, INI-helper behavior, mocked Unreal Editor QA behavior (including fail-closed reporting when an actor class or label cannot be inspected), the map-builder/QA actor contract, and map-builder preflight/cleanup contracts. These Python checks do not execute Unreal Editor.
- A standalone C++17 executable compiles the exact `BatteryInventoryMath.h` header included by the character source, with `-Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined`, and runs 16 battery-inventory arithmetic and accepted/rejected pickup cases.
- A second standalone C++17 executable compiles `StalkerMeleeRules.h` with the same warning and UndefinedBehaviorSanitizer policy and runs 15 impact-predicate cases covering active attack state, range boundaries, controller possession, line of sight, and invalid negative/non-finite geometry inputs.
- A third standalone C++17 executable compiles `SurvivalHealthMath.h`, also with UndefinedBehaviorSanitizer, and runs 16 cases. It checks that overkill reports only the remaining health, that partial-health hits report the actual amount removed, and that non-positive/non-finite damage or invalid health is rejected. The character returns zero for repeat damage after death and does not call the engine damage handler again.
- A fourth standalone C++17 executable compiles `FlashlightBatteryMath.h` with the same warning and UndefinedBehaviorSanitizer policy and runs 25 cases for normalization, ordinary/depleted drain, invalid time/rate values, overflow-resistant large drains, capped recharge, rejected invalid recharge inputs, and aliased input/output behavior.
- A fifth standalone C++17 executable compiles `ObjectiveProgressMath.h` with the same warning and UndefinedBehaviorSanitizer policy and runs 22 cases for target normalization, progress clamping, completion, saturating registration, `INT32_MAX`, and aliased output.

UndefinedBehaviorSanitizer is enabled for all native regression executables so undefined behavior detected during a test terminates the CI step instead of being silently recovered.

These standalone C++ tests validate isolated pure-helper contracts only. They do not compile the Unreal module or exercise Unreal reflection, gameplay, or Blueprint integration.

## What CI does not verify

- Unreal Editor or UnrealBuildTool availability, startup, or project-file generation.
- Compilation of the Unreal C++ module against Unreal Engine 5.4.
- Loading or saving the generated `.umap`, map geometry, collision, navigation generation, or asset references.
- Rendering, input, audio, interaction, AI pathfinding, combat timing, or other runtime gameplay.
- Windows packaging, performance, or a playable build.

Do not describe repository CI as an Unreal Engine build or as proof that the vertical slice runs.

## Local commands

Run the repository checks from the project root:

```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```

Compile and run the isolated battery helper test with GCC and UndefinedBehaviorSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined -I Source/TheLastSignal/Public Tests/test_battery_inventory.cpp -o test_battery_inventory
./test_battery_inventory
```

Compile and run the isolated Stalker melee rules test with GCC and UndefinedBehaviorSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined -I Source/TheLastSignal/Public Tests/test_stalker_melee_rules.cpp -o test_stalker_melee_rules
./test_stalker_melee_rules
```

Compile and run the isolated flashlight battery rules test with GCC and UndefinedBehaviorSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined -I Source/TheLastSignal/Public Tests/test_flashlight_battery_math.cpp -o test_flashlight_battery_math
./test_flashlight_battery_math
```

Compile and run the isolated objective progress rules test with GCC and UndefinedBehaviorSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined -I Source/TheLastSignal/Public Tests/test_objective_progress_math.cpp -o test_objective_progress_math
./test_objective_progress_math
```

Compile and run the isolated survival health rules test with GCC and UndefinedBehaviorSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=undefined -I Source/TheLastSignal/Public Tests/test_survival_health_math.cpp -o test_survival_health_math
./test_survival_health_math
```

## Future Unreal validation

A genuine Unreal validation lane requires an authorized Unreal Engine 5.4 installation and a runner/workstation with compatible OS, memory, storage, and (for editor/rendering tests) GPU support. Do not put Epic credentials or license secrets in repository files. A generic Linux runner is not a substitute for a compatible Unreal workstation.
