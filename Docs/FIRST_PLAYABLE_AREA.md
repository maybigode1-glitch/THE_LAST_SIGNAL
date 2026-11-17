# First playable area — Relay Station 01

## Intent
A compact industrial relay station built as a greybox survival-horror slice. The route starts in the western service bay, passes through a partitioned maintenance hall, and ends at the primary uplink in the eastern chamber.

## What the runtime builder creates
Place one `ASignalPrototypeArea` actor in an otherwise empty level. On BeginPlay it creates:
- A 30 m x 22 m collision-enabled floor and perimeter walls.
- Broken interior partitions and waist-high industrial cover.
- Three interactable relay terminals (the default GameMode objective target is three).
- Two battery-pack pickups.
- One stalker enemy with a temporary cube silhouette (replace with a skeletal mesh Blueprint).
- Editable switches on the builder to disable the stalker, pickups, or objectives while iterating.

The builder uses Unreal's built-in Engine BasicShapes cube. It does not require imported art assets.

## Create the map in Unreal Editor
1. Open `THE_LAST_SIGNAL.uproject` and allow the C++ module to compile.
2. Create a level named `Content/Maps/LVL_RelayStation01`.
3. Add exactly one `ASignalPrototypeArea` actor at (0,0,0).
4. Add a `PlayerStart` at approximately (-1200, 0, 100), facing east (+X).
5. Add a `NavMeshBoundsVolume` covering the floor (approximately 3000 x 2200 cm), raise it enough to cover the floor and walls, then build navigation. Press P to inspect the green navigation area.
6. Set World Settings > GameMode Override to `LastSignalGameMode` (or leave the project default).
7. Set the level as the Editor Startup Map and Game Default Map in Project Settings > Maps & Modes.
8. Press Play. Use WASD/mouse, E to collect/activate, F for flashlight, and R to use a spare battery.

## First-area play loop
- Leave the western service bay and locate the first relay terminal.
- Search the side rooms and cover for the two spare battery packs.
- Avoid the stalker in the maintenance hall; break line of sight around the partitions.
- Activate all three terminals to restore the uplink.

## Greybox coordinates (relative to builder)
- Start: (-1200, 0, 100)
- Relay A: (-250, -650, 0)
- Relay B: (350, 650, 0)
- Primary uplink: (1200, 0, 0)
- Battery packs: (-950, 600, 0) and (500, -700, 0)
- Stalker: (950, 0, 100)

## Current limitations / QA
- This actor builds geometry at runtime; it is not a binary `.umap`. The level, PlayerStart, NavMeshBoundsVolume, and startup-map settings still need to be created in Unreal Editor.
- The stalker silhouette is a temporary static cube. Assign a skeletal mesh, Animation Blueprint, montage, and audio assets using `Docs/STALKER_ANIMATION_AUDIO_SETUP.md`.
- Navigation depends on the editor-built NavMesh. Without a NavMeshBoundsVolume the stalker will not patrol or pathfind correctly.
- The interaction terminals currently use the common interactable base and count toward the three-signal objective.
- No Unreal compilation, editor launch, packaged build, or runtime playtest has been performed in this environment.
