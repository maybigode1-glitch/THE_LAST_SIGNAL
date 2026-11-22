# Development and test environment

## Repository CI lane

GitHub Actions uses a GitHub-hosted Ubuntu runner for checks that do not require Unreal Engine. The current workflow runs Python 3.12 and the system GCC C++ compiler.

## What CI verifies

- `Scripts/validate_project.py` checks the project descriptor, required repository files, and basic Unreal project metadata.
- `python -m unittest discover -s Tests -v` runs the complete Python regression suite. This includes project metadata checks, INI-helper behavior, mocked Unreal Editor QA behavior, the map-builder/QA actor contract, and map-builder preflight/cleanup contracts.
- The map-builder preflight tests use a narrow Unreal API double to verify that missing required assets/classes abort before the existing level is opened or cleared, failed actor deletion is reported, and project startup settings are updated only after the level and asset save calls succeed. They do not execute Unreal Editor.
- A standalone C++17 executable compiles the exact `BatteryInventoryMath.h` header included by the character source, with `-Wall -Wextra -Werror -pedantic`, and runs 16 battery-inventory arithmetic and pickup-acceptance cases.

The standalone C++ test validates the arithmetic helper only. It does not compile the Unreal module or exercise Unreal reflection, gameplay, or Blueprint integration.

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

Compile and run the isolated battery helper test with GCC:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -I Source/TheLastSignal/Public Tests/test_battery_inventory.cpp -o test_battery_inventory
./test_battery_inventory
```

## Future Unreal validation

A genuine Unreal validation lane requires an authorized Unreal Engine 5.4 installation and a runner/workstation with compatible OS, memory, storage, and (for editor/rendering tests) GPU support. Do not put Epic credentials or license secrets in repository files. A generic Linux runner is not a substitute for a compatible Unreal workstation.
