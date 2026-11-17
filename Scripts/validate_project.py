#!/usr/bin/env python3
"""Repository-level checks for THE LAST SIGNAL; does not compile Unreal Engine."""
from __future__ import annotations
import json
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
REQUIRED_FILES = (
    "THE_LAST_SIGNAL.uproject", "Config/DefaultEngine.ini", "Config/DefaultInput.ini",
    "Source/TheLastSignal/TheLastSignal.Build.cs", "Source/TheLastSignal/Private/TheLastSignal.cpp",
    "Source/TheLastSignal/Public/LastSignalCharacter.h", "Source/TheLastSignal/Private/LastSignalCharacter.cpp",
    "Source/TheLastSignal/Public/SignalInteractable.h", "Source/TheLastSignal/Private/SignalInteractable.cpp",
    "Source/TheLastSignal/Public/SignalBatteryPickup.h", "Source/TheLastSignal/Private/SignalBatteryPickup.cpp",
    "Source/TheLastSignal/Public/LastSignalGameMode.h", "Source/TheLastSignal/Private/LastSignalGameMode.cpp",
    "Source/TheLastSignal/Public/LastSignalHUD.h", "Source/TheLastSignal/Private/LastSignalHUD.cpp",\n    "Source/TheLastSignal/Public/SignalStalker.h", "Source/TheLastSignal/Private/SignalStalker.cpp",
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
        "Source/TheLastSignal/Private/LastSignalHUD.cpp": ("GetFlashlightBatteryPercent", "GetBatteryPackCount", "GetSignalsCollected", "GetRequiredSignals", "DrawHUD", "FLASHLIGHT", "SPARE BATTERIES"),
        "Source/TheLastSignal/Private/SignalInteractable.cpp": ("RegisterSignalCollected", "bHasBeenActivated"),\n        "Source/TheLastSignal/Private/SignalStalker.cpp": ("AAIController", "LineOfSightTo", "MoveToActor", "MoveToLocation", "PerformAttack", "TakeDamage", "GetRandomReachablePointInRadius"),\n        "Source/TheLastSignal/Private/LastSignalCharacter.cpp": ("USpotLightComponent", "ToggleFlashlight", "FlashlightDrainPerSecond", "AddBatteryPack", "UseBatteryPack", "TakeDamage", "Health", "bIsDead", "YOU DIED"),\n        "Source/TheLastSignal/Private/LastSignalHUD.cpp": ("GetHealthPercent", "HEALTH"),
    }
    for relative, tokens in checks.items():
        path = root / relative
        if path.is_file():
            source = path.read_text(encoding="utf-8")
            for token in tokens:
                if token not in source:
                    errors.append(f"{relative} missing expected element: {token}")
    return errors
def main() -> int:
    errors = validate_repository(ROOT)
    if errors:
        print("VALIDATION FAILED")
        for error in errors:
            print(f" - {error}")
        return 1
    print("VALIDATION PASSED: project metadata, required source files, inventory, pickups, objectives, HUD, enemy AI, health, and input mappings.")
    print("Scope: static repository checks only; no Unreal build or runtime test was performed.")
    return 0
if __name__ == "__main__":
    sys.exit(main())
