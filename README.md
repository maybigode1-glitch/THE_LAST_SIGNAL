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
- `ASignalPrototypeArea` runtime greybox builder source.
- Unreal Editor Python automation creates a saved `LVL_RelayStation01` map with editor-persistent floor/walls, partitions, cover, terminals, battery pickups, PlayerStart, NavMeshBoundsVolume, and a stalker AI actor.

## Automatically create the first map
1. Open `THE_LAST_SIGNAL.uproject` in Unreal Engine 5.4 and let the C++ module compile.
2. In the Editor, run `Scripts/create_relay_station_map.py` using **Tools > Execute Python Script**.
3. The script creates/saves `/Game/Maps/LVL_RelayStation01`, updates Editor Startup Map and Game Default Map in `Config/DefaultEngine.ini`, and submits an editor build where supported.
4. Press **P** to inspect NavMesh. If it is not built yet, use **Build > Build Paths**.
5. Press Play. Use WASD/mouse, E to interact, F for flashlight, and R to use a spare battery.

The script is idempotent for its own generated actors: rerunning it opens the existing level and replaces actors with the `RS01_AUTO_` label prefix. It does not delete unrelated level actors.

Full instructions: [Docs/FIRST_PLAYABLE_AREA.md](Docs/FIRST_PLAYABLE_AREA.md).

Enemy mesh, Animation Blueprint, montage, and sound assignment: [Docs/STALKER_ANIMATION_AUDIO_SETUP.md](Docs/STALKER_ANIMATION_AUDIO_SETUP.md).

## Repository checks
```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```
GitHub Actions runs repository-level validation and Python unit tests on pushes and pull requests.

## Honest test boundary
The editor automation script is committed but has not been executed in an Unreal Editor in this environment. No generated `.umap`, Unreal compilation, editor launch, runtime playtest, or Windows packaging is claimed until verified. See [Docs/TEST_STATUS.md](Docs/TEST_STATUS.md).
