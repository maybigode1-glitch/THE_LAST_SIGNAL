#!/usr/bin/env python3
"""Repository-level checks for THE LAST SIGNAL; does not compile Unreal Engine."""
from __future__ import annotations
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def validate_uproject(path: Path) -> list[str]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"Invalid .uproject JSON: {exc}"]
    errors: list[str] = []
    if not isinstance(data, dict):
        errors.append(".uproject root must be a JSON object")
    elif "EngineAssociation" not in data:
        errors.append("Missing EngineAssociation in .uproject")
    elif not isinstance(data["EngineAssociation"], (str, int)):
        errors.append("EngineAssociation must be a string or number")
    return errors

def main() -> int:
    errors: list[str] = []
    projects = sorted(ROOT.glob("*.uproject"))
    if not projects:
        print("INFO: no .uproject file found at repository root (scaffold may be incomplete)")
    for project in projects:
        errors.extend(f"{project.name}: {e}" for e in validate_uproject(project))
    for directory in ("Config", "Source", "Docs", "Scripts", "Tests"):
        if not (ROOT / directory).is_dir():
            print(f"WARNING: expected directory missing: {directory}")
    if errors:
        print("VALIDATION FAILED")
        for error in errors:
            print(f" - {error}")
        return 1
    print(f"VALIDATION PASSED: {len(projects)} .uproject file(s) checked")
    print("Scope: repository metadata only; no Unreal build or runtime test was performed.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
