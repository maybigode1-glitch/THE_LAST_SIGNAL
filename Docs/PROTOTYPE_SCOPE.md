# Prototype — The Awakening

## Implemented in C++ source
- First-person character, camera, WASD movement, and mouse look.
- E interaction using a Visibility-channel line trace.
- F flashlight with finite charge; it drains while switched on and turns off at zero.
- Battery-pack inventory: interact with a battery pickup using E; press R to consume one pack and restore up to 35 percentage points of flashlight charge.
- Blueprintable `ASignalBatteryPickup` actor. Set `PacksGranted` in a child Blueprint to change the pickup amount.
- Signal objectives: each successfully activated standard `ASignalInteractable` increments progress in `ALastSignalGameMode`. The default objective is 3 signals; `RequiredSignals` is configurable in a GameMode Blueprint.
- Blueprint-readable getters expose flashlight charge, inventory count, and objective progress/completion.
- Native C++ survival HUD (`ALastSignalHUD`) draws flashlight percentage and state, spare battery count, player health, signal objective progress, completion state, and a small center reticle. It is assigned by default in `ALastSignalGameMode`.
- Blueprintable `ASignalStalker` enemy uses an `AAIController` and navigation: it patrols near its spawn point, detects the player by line of sight inside its detection radius, pursues while visible, investigates the last known position after losing sight, then returns to its patrol area.
- The stalker performs timed melee attacks inside its attack radius. Damage is applied through Unreal's damage system.
- Player health starts at 100; reaching zero triggers the death state, disables movement/look and switches off the flashlight. Health is displayed in the HUD.
- Stalker tuning is exposed to Blueprints: detection radius, attack range, attack interval, damage, patrol radius and movement speed (via CharacterMovement).

## Input
| Key | Action |
|---|---|
| WASD | Move |
| Mouse | Look |
| E | Interact / collect |
| F | Toggle flashlight |
| R | Use one battery pack |

## Editor setup still required
1. Open `THE_LAST_SIGNAL.uproject` with Unreal Engine 5.4 or a compatible 5.x installation.
2. Generate IDE project files and compile the TheLastSignal module.
3. Create `Content/Maps/LVL_Prototype`, add a PlayerStart, and set it as the startup map.
4. Create a child Blueprint of `ASignalInteractable` for a signal objective; assign a visible Static Mesh that blocks Visibility.
5. Create a child Blueprint of `ASignalBatteryPickup`; assign a battery-shaped mesh that blocks Visibility. Place several in the level.
6. Optionally create a child GameMode Blueprint and set the required signal count.
7. Add a `NavMeshBoundsVolume` covering walkable floor space and build navigation (press P in the editor to inspect the green navigable area).
8. Place one or more `ASignalStalker` actors inside the navigable area. Create a child Blueprint to assign a skeletal mesh, animation Blueprint, and optional sounds; the native class supplies AI behaviour but intentionally does not bundle copyrighted character assets.
9. Set the map's GameMode override to `ALastSignalGameMode` or its Blueprint child, then test E/F/R and enemy behaviour. The HUD is registered automatically by this GameMode.

## Acceptance checklist
- [ ] Project opens and C++ module compiles in UE 5.4.
- [ ] Character spawns and movement/look work.
- [ ] E collects a battery pack once; the pickup disappears.
- [ ] R consumes a pack only when flashlight charge is below 100%; charge increases by up to 35.
- [ ] F toggles the light and charge drains while active.
- [ ] HUD displays live flashlight charge, inventory count, and signal progress.
- [ ] Activating each signal increments objective progress once; completion occurs at the configured target.

## Scope boundary
These are source-code systems, not a finished level or playable release. No binary Unreal assets, map, audio, enemy AI, save system, packaging, or runtime QA are included yet.
