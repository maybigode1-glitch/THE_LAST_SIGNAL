# THE LAST SIGNAL

First-person survival-horror prototype built with Unreal Engine 5.4.

## Current prototype source
- First-person character, camera, WASD movement, and mouse look.
- E interaction via Visibility-channel line trace.
- F flashlight with a draining battery.
- Battery-pack pickup actor: collect with E and store packs in the character inventory.
- Press R to consume one pack and restore up to 35 percentage points of flashlight charge.
- Signal objective counter in the GameMode; standard interactables count once toward the configurable target (default: 3).
- Blueprint getters for flashlight charge, battery-pack count, and objective progress/completion.
- Native C++ survival HUD showing flashlight charge/state, spare battery inventory, player health, signal objective progress/completion, and a center reticle.
- `ASignalStalker` enemy: AIController-driven navigation, patrol around its spawn point, line-of-sight detection, player pursuit, last-known-position search, and timed melee attacks.
- Player health (100 by default), damage feedback, and death state; movement/look and flashlight controls are disabled on death.
- AI/navigation module dependencies are enabled in the runtime module.

The repository does not yet contain binary Unreal assets or a playable map. Create the test map and Blueprint instances in Unreal Editor using [Docs/PROTOTYPE_SCOPE.md](Docs/PROTOTYPE_SCOPE.md).

## Open in Unreal
1. Install Unreal Engine 5.4 (or a compatible 5.x version).
2. Open `THE_LAST_SIGNAL.uproject`.
3. Allow Unreal to generate IDE project files and compile the C++ module.
4. Follow the editor setup checklist in [Docs/PROTOTYPE_SCOPE.md](Docs/PROTOTYPE_SCOPE.md).

## Repository checks
```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```
GitHub Actions runs repository-level validation and Python unit tests on pushes and pull requests.

## Honest test boundary
Passing repository checks does not prove that Unreal C++ compiled, the editor launched, gameplay ran, or a Windows build was packaged. See [Docs/TEST_STATUS.md](Docs/TEST_STATUS.md).
