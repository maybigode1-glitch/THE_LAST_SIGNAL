#!/usr/bin/env python3
"""Repository-level checks for THE LAST SIGNAL; does not compile Unreal Engine."""
from __future__ import annotations
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REQUIRED_FILES = (
    "THE_LAST_SIGNAL.uproject",
    "Config/DefaultEngine.ini",
    "Config/DefaultInput.ini",
    "Source/TheLastSignal/TheLastSignal.Build.cs",
    "Source/TheLastSignal/Private/TheLastSignal.cpp",
    "Source/TheLastSignal/Public/LastSignalCharacter.h",
    "Source/TheLastSignal/Private/LastSignalCharacter.cpp",
    "Source/TheLastSignal/Public/SignalInteractable.h",
    "Source/TheLastSignal/Private/SignalInteractable.cpp",
    "Source/TheLastSignal/Public/LastSignalGameMode.h",
    "Source/TheLastSignal/Private/LastSignalGameMode.cpp",
)

def validate_uproject(path: Path) -> list[str]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"Invalid .uproject JSON: {exc}"]
    errors: list[str] = []
    if not isinstance(data, dict):
        return [".uproject root must be a JSON object"]
    if not isinstance(data.get("EngineAssociation"), (str, int)):
        errors.append("EngineAssociation must be a string or number")
    modules = data.get("Modules")
    if not isinstance(modules, list) or not any(
        isinstance(module, dict) and module.get("Name") == "TheLastSignal"
        for module in modules
    ):
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
    return errors

def main() -> int:
    errors = validate_repository(ROOT)
    if errors:
        print("VALIDATION FAILED")
        for error in errors:
            print(f" - {error}")
        return 1
    print("VALIDATION PASSED: project descriptor, module declaration, config, and core source files found.")
    print("Scope: repository structure and metadata only; no Unreal build or runtime test was performed.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
