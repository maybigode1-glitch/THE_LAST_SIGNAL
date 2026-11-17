# THE LAST SIGNAL

First-person survival-horror prototype built with Unreal Engine 5.4.

## Current prototype source
- First-person character, camera, WASD movement, and mouse look.
- E interaction via Visibility-channel line trace.
- F flashlight with a draining battery.
- Battery-pack pickup actor: collect with E and store packs in the character inventory.
- Press R to consume one pack and restore up to 35 percentage points of flashlight charge.
- Signal objective counter in the GameMode; standard interactables count once toward the configurable target (default: 3).
- Native C++ survival HUD showing flashlight charge/state, spare battery inventory, player health, signal objective progress/completion, and a center reticle.
- `ASignalStalker` enemy: navigation patrol, line-of-sight detection, pursuit, last-known-position search, and timed melee attacks.
- `USignalStalkerAnimInstance` exposes locomotion/chase/search/attack state; montage and six enemy audio hooks are configurable.
- `ASignalPrototypeArea` builds the first industrial relay-station greybox at runtime: collision floor/walls, partitions, cover, three signal terminals, two battery pickups, and a temporary stalker silhouette.

## First area
Follow [Docs/FIRST_PLAYABLE_AREA.md](Docs/FIRST_PLAYABLE_AREA.md) to create `LVL_RelayStation01`, place the area builder and PlayerStart, add a NavMeshBoundsVolume, and set the startup map. The builder uses Unreal's built-in cube mesh and needs no imported environment art.

Enemy mesh, Animation Blueprint, montage, and sound assignment: [Docs/STALKER_ANIMATION_AUDIO_SETUP.md](Docs/STALKER_ANIMATION_AUDIO_SETUP.md).

## Open in Unreal
1. Install Unreal Engine 5.4 (or a compatible 5.x version).
2. Open `THE_LAST_SIGNAL.uproject`.
3. Allow Unreal to generate IDE project files and compile the C++ module.
4. Follow [Docs/FIRST_PLAYABLE_AREA.md](Docs/FIRST_PLAYABLE_AREA.md).

## Repository checks
```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```
GitHub Actions runs repository-level validation and Python unit tests on pushes and pull requests.

## Honest test boundary
The area builder is source code, not a binary `.umap`. No Unreal compilation, editor launch, runtime playtest, or Windows packaging is claimed until verified. See [Docs/TEST_STATUS.md](Docs/TEST_STATUS.md).
