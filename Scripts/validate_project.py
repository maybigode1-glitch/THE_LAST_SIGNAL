#!/usr/bin/env python3
"""Repository-level checks for THE LAST SIGNAL; does not compile Unreal Engine."""
from __future__ import annotations
import json
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
REQUIRED_FILES = (
    "THE_LAST_SIGNAL.uproject", "Config/DefaultEngine.ini", "Config/DefaultInput.ini",
    "Scripts/create_relay_station_map.py",
    "Source/TheLastSignal/TheLastSignal.Build.cs", "Source/TheLastSignal/Private/TheLastSignal.cpp",
    "Source/TheLastSignal/Public/LastSignalCharacter.h", "Source/TheLastSignal/Private/LastSignalCharacter.cpp",
    "Source/TheLastSignal/Public/SignalInteractable.h", "Source/TheLastSignal/Private/SignalInteractable.cpp",
    "Source/TheLastSignal/Public/SignalBatteryPickup.h", "Source/TheLastSignal/Private/SignalBatteryPickup.cpp",
    "Source/TheLastSignal/Public/LastSignalGameMode.h", "Source/TheLastSignal/Private/LastSignalGameMode.cpp",
    "Source/TheLastSignal/Public/LastSignalHUD.h", "Source/TheLastSignal/Private/LastSignalHUD.cpp",
    "Source/TheLastSignal/Public/SignalStalker.h", "Source/TheLastSignal/Private/SignalStalker.cpp",
    "Source/TheLastSignal/Public/SignalStalkerAnimInstance.h", "Source/TheLastSignal/Private/SignalStalkerAnimInstance.cpp",
    "Source/TheLastSignal/Public/SignalPrototypeArea.h", "Source/TheLastSignal/Private/SignalPrototypeArea.cpp",
    "Docs/FIRST_PLAYABLE_AREA.md",
)
def validate_uproject(path: Path) -> list[str]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"Invalid .uproject JSON: {exc}"]
    if not isinstance(data, dict):
        return [".uproject root must be a JSON object"]
    errors: list[str] = []
    if not isinstance(data.get("EngineAssociation"), (str, int)):
        errors.append("EngineAssociation must be a string or number")
    modules = data.get("Modules")
    if not isinstance(modules, list) or not any(isinstance(m, dict) and m.get("Name") == "TheLastSignal" for m in modules):
        errors.append("Modules must declare the TheLastSignal runtime module")
    plugins = {p.get("Name"): p.get("Enabled") for p in data.get("Plugins", []) if isinstance(p, dict)}
    for plugin in ("PythonScriptPlugin", "EditorScriptingUtilities"):
        if plugins.get(plugin) is not True:
            errors.append(f"Required editor automation plugin not enabled: {plugin}")
    return errors
def validate_repository(root: Path) -> list[str]:
    errors: list[str] = []
    for relative in REQUIRED_FILES:
        if not (root / relative).is_file():
            errors.append(f"Required project file missing: {relative}")
    project = root / "THE_LAST_SIGNAL.uproject"
    if project.is_file():
        errors.extend(validate_uproject(project))
    input_config = root / "Config/DefaultInput.ini"
    if input_config.is_file():
        config = input_config.read_text(encoding="utf-8")
        for mapping in ('ActionName="Flashlight",Key=F', 'ActionName="UseBattery",Key=R'):
            if mapping not in config:
                errors.append(f"Missing input mapping: {mapping}")
    checks = {
        "Source/TheLastSignal/Private/LastSignalCharacter.cpp": ("USpotLightComponent", "ToggleFlashlight", "FlashlightDrainPerSecond", "AddBatteryPack", "UseBatteryPack"),
        "Source/TheLastSignal/Private/SignalBatteryPickup.cpp": ("ASignalBatteryPickup", "AddBatteryPack", "Destroy()"),
        "Source/TheLastSignal/Private/LastSignalGameMode.cpp": ("RegisterSignalCollected", "AreObjectivesComplete", "ALastSignalHUD::StaticClass"),
        "Source/TheLastSignal/Private/LastSignalHUD.cpp": ("GetFlashlightBatteryPercent", "GetBatteryPackCount", "GetSignalsCollected", "GetRequiredSignals", "DrawHUD", "FLASHLIGHT", "SPARE BATTERIES", "GetHealthPercent"),
        "Source/TheLastSignal/Private/SignalInteractable.cpp": ("RegisterSignalCollected", "bHasBeenActivated"),
        "Source/TheLastSignal/Private/SignalStalker.cpp": ("AAIController", "LineOfSightTo", "MoveToActor", "MoveToLocation", "PerformAttack", "TakeDamage", "GetRandomReachablePointInRadius", "FootstepSound", "AlertSound", "AttackSound", "SearchSound", "AmbientLoopSound", "ChaseLoopSound", "Montage_Play", "bIsAttacking"),
        "Source/TheLastSignal/Private/SignalStalkerAnimInstance.cpp": ("NativeUpdateAnimation", "GroundSpeed", "IsChasing", "IsSearching", "IsAttacking"),
        "Source/TheLastSignal/Private/SignalPrototypeArea.cpp": ("BuildGeometry", "SpawnObjectivesAndPickups", "SpawnStalker", "ASignalBatteryPickup", "ASignalInteractable", "ASignalStalker", "BasicShapes/Cube.Cube"),
    }
    for relative, tokens in checks.items():
        path = root / relative
        if path.is_file():
            source = path.read_text(encoding="utf-8")
            for token in tokens:
                if token not in source:
                    errors.append(f"{relative} missing expected element: {token}")
    automation = root / "Scripts/create_relay_station_map.py"
    if automation.is_file():
        source = automation.read_text(encoding="utf-8")
        for token in ("LVL_RelayStation01", "PlayerStart", "NavMeshBoundsVolume", "GameDefaultMap", "EditorStartupMap", "save_current_level", "RuntimeGeneration", "Dynamic", "SignalBatteryPickup", "SignalInteractable", "SignalStalker"):
            if token not in source:
                errors.append(f"Scripts/create_relay_station_map.py missing expected element: {token}")
    hud_path = root / "Source/TheLastSignal/Private/LastSignalHUD.cpp"
    if hud_path.is_file() and "\\n        const float HealthY" in hud_path.read_text(encoding="utf-8"):
        errors.append("LastSignalHUD.cpp contains literal escaped newlines in the health HUD block")
    return errors
def main() -> int:
    errors = validate_repository(ROOT)
    if errors:
        print("VALIDATION FAILED")
        for error in errors:
            print(f" - {error}")
        return 1
    print("VALIDATION PASSED: UE metadata, editor map automation, first-area builder, objectives, pickups, HUD, enemy AI, animation/audio hooks, and input mappings.")
    print("Scope: static repository checks only; no Unreal build or runtime test was performed.")
    return 0
if __name__ == "__main__":
    sys.exit(main())
