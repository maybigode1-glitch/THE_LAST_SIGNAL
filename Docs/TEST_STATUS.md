# Test status

## Implemented in repository source
- [x] UE5 project descriptor and runtime module.
- [x] First-person character, movement, mouse look, and flashlight battery drain.
- [x] E-key interactable base actor and Blueprint-native interaction event.
- [x] Battery pickup actor and R-key inventory use.
- [x] Battery-pack inventory addition uses a shared overflow-safe int64 intermediate and retains the 0..99 cap.
- [x] Battery pickups are consumed only when the inventory count increases; a full inventory leaves the pickup available.
- [x] Standalone native C++ regression cases for ordinary additions, cap behavior, accepted/rejected pickup additions, non-positive amounts, and int32 boundary values.
- [x] GameMode objective counter and configurable target (default 3).
- [x] Native C++ survival HUD source for flashlight, inventory, health, objectives, and reticle.
- [x] Stalker AI source for patrol, sight detection, pursuit, search, and melee attacks.
- [x] Melee attack wind-up: impact requires the attack state to remain active and rechecks range and line of sight with a valid AI controller before damage can be applied.
- [x] Standalone C++ regression cases for Stalker melee range boundaries, controller possession, line of sight, invalid geometry, and expired attack-state cancellation (15 cases).
- [x] Visible cube placeholder for the stalker when no skeletal mesh is assigned.
- [x] Player health, damage, death state, and HUD health display.
- [x] Animation instance state and montage hooks for the stalker.
- [x] Configurable enemy audio hooks for footsteps, alert, attack, search, ambient, and chase.
- [x] Runtime greybox builder source for Relay Station 01.
- [x] Unreal Editor Python automation script for persistent level geometry, objectives, pickups, PlayerStart, NavMeshBoundsVolume, stalker AI, and map startup configuration.
- [x] Map-builder preflight resolves required mesh/classes before opening or replacing the existing level.
- [x] Generated-actor cleanup failures are reported and stop regeneration instead of being silently ignored.
- [x] Project startup-map settings are updated only after the level and map asset save calls succeed.
- [x] PythonScriptPlugin and EditorScriptingUtilities enabled in the project descriptor.
- [x] First-area automation guide and repository validator/test coverage.
- [x] Unreal Editor structural QA script for the generated Relay Station map, with a JSON report and explicit pass/fail checks.
- [x] Offline unit tests for the Editor QA report logic: passing actor set, missing stalker, wrong map, and missing required generated labels.
- [x] Offline QA regression tests for duplicate generated labels, required labels assigned to unexpected actor classes, and actor class/label inspection failures; all failures are recorded in the JSON report.
- [x] AST-based contract test verifies that the map builder creates exactly the eight QA-required generated actors with their expected Unreal class names.
- [x] Offline map-builder preflight tests verify missing assets/classes abort before level mutation, failed generated-actor deletion is surfaced, and startup settings are written after save calls.
- [x] Offline tests for the map builder INI helper: update existing keys while preserving unrelated settings, create a missing section, and repeat updates idempotently.

## Automated validation
- [x] GitHub Actions workflow runs repository metadata validation, the complete Python unit-test suite, the standalone battery inventory C++ regression executable (16 cases), and the standalone Stalker melee rules C++ regression executable (15 cases). See the latest checks on [PR #2](https://github.com/maybigode1-glitch/THE_LAST_SIGNAL/pull/2).

## Validation still pending
- [ ] Run `Scripts/qa_relay_station.py` inside Unreal Editor after generating the map; inspect `Saved/RelayStation01_QA.json`.
- [ ] Generate project files and compile with UnrealBuildTool on UE 5.4.
- [ ] Run `Scripts/create_relay_station_map.py` inside Unreal Editor and verify that the `.umap` is saved.
- [ ] Verify PlayerStart spawn and GameMode/HUD selection.
- [ ] Verify NavMesh generation (press P; use Build > Build Paths if needed).
- [ ] Verify collision, interaction reach, three terminals, two pickups, and stalker pathfinding in runtime.
- [ ] Verify the stalker wind-up/impact timing and confirm the player can dodge a missed melee strike.
- [ ] Assign skeletal mesh, Animation Blueprint, attack montage, and audio assets; test in runtime.
- [ ] Produce Windows packaged build and perform gameplay QA.

The offline Editor-QA unit tests use a small Unreal Python API double to test the script's report/pass/fail behavior; they do not execute Unreal Editor. The map-builder preflight tests use the same limited approach to check failure ordering and cleanup error handling, not actual level operations. The standalone battery inventory and Stalker melee rules tests compile and execute pure C++ headers used by the game source, but do not compile the Unreal module. The INI-helper tests use standard-library temporary files and do not execute Unreal Editor. Repository checks do not prove Unreal compilation, editor startup, map generation, runtime behavior, or packaging.
