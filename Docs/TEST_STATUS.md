# Test status

## Implemented in repository source
- [x] UE5 project descriptor and runtime module.
- [x] First-person character, movement, mouse look, and flashlight battery drain.
- [x] E-key interactable base actor and Blueprint-native interaction event.
- [x] Battery pickup actor that adds packs to character inventory and destroys itself after collection.
- [x] R-key inventory use; each pack restores up to 35 percentage points of flashlight charge.
- [x] GameMode objective counter, configurable target (default 3), and completion state.
- [x] Native C++ survival HUD source displaying flashlight charge, battery pack inventory, player health, and signal objective progress; registered as the GameMode default HUD.
- [x] `ASignalStalker` C++ enemy with navigation patrol, line-of-sight detection, pursuit, last-known-position search, and timed melee attacks.
- [x] Player damage, health depletion, death state, and health HUD display.
- [x] Runtime module dependencies include AIModule and NavigationSystem.
- [x] `USignalStalkerAnimInstance` exposes movement, chase, search, attack and death flags plus ground speed for Animation Blueprint graphs.
- [x] Enemy attack can trigger an assigned Animation Montage.
- [x] Configurable enemy audio hooks cover footsteps, alert, attack, search, ambient loop and chase loop.
- [x] Footsteps are distance-triggered; one-shot sounds play at the enemy location; loop audio changes with chase state.
- [x] Static validator covers required source files, input mappings, pickup/inventory/objective source contracts.
- [x] Python unit tests cover repository contract markers.

## Validation still pending
- [ ] Execute Python validator and unit tests in a local/hosted environment and record results.
- [ ] Review a successful GitHub Actions run.
- [ ] Generate project files and compile with UnrealBuildTool on UE 5.4.
- [ ] Open in Unreal Editor and create the test map/Blueprint instances.
- [ ] Place a NavMeshBoundsVolume covering the playable area and add one or more `ASignalStalker` actors; build navigation data.
- [ ] Assign skeletal mesh, Animation Blueprint, locomotion clips and attack montage in Unreal Editor; test animation transitions.
- [ ] Assign sound assets and test footsteps, alert, attack, search, ambient loop and chase loop in runtime.
- [ ] Exercise enemy patrol, sight detection, pursuit, search, melee damage, player death, and health HUD in runtime.
- [ ] Exercise pickup, inventory use, flashlight, objective completion, and HUD updates in runtime.
- [ ] Produce Windows packaged build and perform gameplay QA.

Repository checks are static/Python checks only. They do not prove Unreal compilation, editor startup, runtime behavior, or packaging.
