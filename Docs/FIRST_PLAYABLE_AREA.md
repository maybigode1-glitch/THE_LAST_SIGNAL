# First playable area — Relay Station 01

## Intent
A compact industrial relay station greybox. The route starts in the western service bay, passes through a partitioned maintenance hall, and ends at the primary uplink in the eastern chamber.

## Automatic map creation (recommended)
The project enables Unreal's Python and Editor Scripting plugins. After opening the project and compiling the C++ module in Unreal Engine 5.4:

1. In Unreal Editor choose **Tools > Execute Python Script**.
2. Select `Scripts/create_relay_station_map.py` from the project directory.
3. Wait for the Output Log message `SUCCESS: saved /Game/Maps/LVL_RelayStation01.LVL_RelayStation01`.
4. The script creates or opens `Content/Maps/LVL_RelayStation01`, then places editor-persistent cube geometry, three interactable signal terminals, two battery pickups, a PlayerStart, a NavMeshBoundsVolume, and the stalker AI class.
5. It updates `Config/DefaultEngine.ini` with both `EditorStartupMap` and `GameDefaultMap`, retaining `LastSignalGameMode` as the global GameMode.
6. Press **P** to display navigation. If navigation is not yet generated, choose **Build > Build Paths**, wait for completion, then press **P** again.
7. Press Play.

The script can be run again safely: it removes and recreates only actors whose labels begin with `RS01_AUTO_`; unrelated actors are left untouched.

## What the script creates
- A 30 m x 22 m collision-enabled floor and perimeter walls.
- Broken interior partitions and waist-high industrial cover.
- Three interactable relay terminals (default GameMode objective target: three).
- Two battery-pack pickups.
- A PlayerStart at (-1200, 0, 100), facing +X.
- A NavMeshBoundsVolume centered over the floor and scaled to cover the play area.
- One stalker AI actor in the eastern maintenance hall.

The environment uses Unreal's built-in Engine BasicShapes cube; no imported environment art is required. The stalker still needs its skeletal mesh, Animation Blueprint, montage, and sound assets assigned.

## Manual fallback
If Python automation is unavailable:
1. Create `Content/Maps/LVL_RelayStation01`.
2. Add one `ASignalPrototypeArea` actor at (0,0,0), a PlayerStart at (-1200,0,100), and a NavMeshBoundsVolume over the 30 m x 22 m floor.
3. Build Paths and set the map as Editor Startup Map and Game Default Map in Project Settings > Maps & Modes.

## First-area play loop
- Leave the western service bay and locate the first relay terminal.
- Search side rooms and cover for the two spare battery packs.
- Avoid the stalker in the maintenance hall; break line of sight around partitions.
- Activate all three terminals to restore the uplink.

## Greybox coordinates
- PlayerStart: (-1200, 0, 100)
- Relay A: (-250, -650, 55)
- Relay B: (350, 650, 55)
- Primary uplink: (1200, 0, 55)
- Battery packs: (-950, 600, 30) and (500, -700, 30)
- Stalker: (950, 0, 100)

## Current limitations / QA
- The Python script must be run inside Unreal Editor; ordinary system Python cannot create a valid `.umap`.
- NavMeshBoundsVolume is placed automatically and the script sets Recast `RuntimeGeneration=Dynamic`, so the navigation mesh can be generated at runtime inside its bounds. The script also requests an editor build where that API is available. For editor visualization or a static bake, use **Build > Build Paths** and inspect with **P**. Dynamic generation is a fallback for playability, not proof that a baked navmesh was produced.
- The stalker AI actor has no assigned skeletal mesh in this greybox map; it may be invisible until art is assigned.
- No Unreal compilation, script execution, editor launch, packaged build, or runtime playtest has been performed in this environment.
