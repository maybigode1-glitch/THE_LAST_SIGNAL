# Prototype — The Awakening

## Implemented in C++ source
- First-person character, camera, WASD movement, and mouse look.
- E interaction using a Visibility-channel line trace.
- F flashlight with finite charge; it drains while switched on and turns off at zero.
- Battery-pack inventory: interact with a battery pickup using E; press R to consume one pack and restore up to 35 percentage points of flashlight charge.
- Blueprintable `ASignalBatteryPickup` actor.
- Signal objectives: each successfully activated standard `ASignalInteractable` increments progress in `ALastSignalGameMode`. Default target: 3.
- Native C++ survival HUD displays flashlight, inventory, health, signal progress, and a reticle.
- `ASignalStalker` patrols, detects by line of sight, pursues, searches last-known location, and attacks.
- `USignalStalkerAnimInstance` exposes movement/chase/search/attack state; montage and six audio hooks are configurable.
- `ASignalPrototypeArea` procedurally assembles the Relay Station 01 greybox at runtime using the Engine BasicShapes cube: floor, perimeter, broken partitions, cover, three signal terminals, two battery pickups, and a temporary enemy silhouette.

## Input
| Key | Action |
|---|---|
| WASD | Move |
| Mouse | Look |
| E | Interact / collect |
| F | Toggle flashlight |
| R | Use one battery pack |

## First playable area setup
Follow [FIRST_PLAYABLE_AREA.md](FIRST_PLAYABLE_AREA.md). In summary:
1. Compile the C++ project in Unreal Engine 5.4.
2. Create `Content/Maps/LVL_RelayStation01`.
3. Place one `ASignalPrototypeArea` actor at the origin.
4. Place a `PlayerStart` at approximately (-1200, 0, 100), facing +X.
5. Add a `NavMeshBoundsVolume` over the 30 m x 22 m floor and build navigation.
6. Use `LastSignalGameMode` and set the level as the startup/game default map.
7. Press Play and test movement, E/F/R, the three terminals, pickups, and stalker.

## Enemy art setup
See [STALKER_ANIMATION_AUDIO_SETUP.md](STALKER_ANIMATION_AUDIO_SETUP.md) to assign a skeletal mesh, Animation Blueprint, attack montage, and audio assets.

## Acceptance checklist
- [ ] Project opens and C++ module compiles in UE 5.4.
- [ ] Relay Station 01 level is created with builder, PlayerStart, and NavMeshBoundsVolume.
- [ ] Character spawns and movement/look work.
- [ ] E activates each of three terminals once; objective completes at 3/3.
- [ ] E collects both battery packs; R restores flashlight charge.
- [ ] F toggles the light and charge drains while active.
- [ ] Stalker navigates, detects, chases, searches, and damages the player.
- [ ] HUD displays live flashlight, inventory, health, and objective status.
- [ ] Enemy mesh, animations, and sounds are assigned and tested.

## Scope boundary
The runtime builder is C++ source, not a saved binary `.umap`. The level still needs to be created in Unreal Editor, and navigation needs a placed/built NavMeshBoundsVolume. The stalker silhouette is temporary. No Unreal compilation, editor startup, packaged build, or runtime playtest has been performed in this environment.
