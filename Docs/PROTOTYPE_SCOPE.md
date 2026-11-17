# Prototype 0.1 — The Awakening

## Goal
Establish a small first-person horror sandbox that can later host the opening sequence.

## Implemented in source
- First-person character with capsule collision and camera.
- Keyboard movement (WASD) and mouse look.
- E key interaction using a visibility-channel line trace.
- F key flashlight attached to the camera, with a finite battery that drains while switched on and forces the light off at zero.
- C++ interactable actor with a Blueprint-native interaction event.
- GameMode selecting the first-person character.

## Editor setup still required
1. Open THE_LAST_SIGNAL.uproject with Unreal Engine 5.4 or a compatible 5.x installation.
2. Allow Unreal to generate IDE project files and compile the TheLastSignal module.
3. Create a level at Content/Maps/LVL_Prototype.
4. Place a PlayerStart and a child Blueprint of ASignalInteractable.
5. Assign a static mesh to the Blueprint Mesh component. Ensure it blocks Visibility.
6. Set the new level as the editor startup map and test WASD, mouse look, and E interaction.

## Acceptance checklist
- [ ] Project opens in Unreal Editor without module errors.
- [ ] C++ module compiles on the target Unreal version.
- [ ] Character spawns at PlayerStart.
- [ ] WASD movement and mouse look work.
- [ ] E triggers interaction only when looking at an interactable within range.
- [ ] Blueprint override of Interact executes.

## Scope boundary
This is a source-code prototype, not a finished level or playable release. No assets, lighting, sound, save system, enemy AI, menus, packaging, or runtime QA are included yet.
