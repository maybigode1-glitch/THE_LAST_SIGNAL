"""Run Relay Station 01 structural QA inside Unreal Editor 5.4.

Run after opening /Game/Maps/LVL_RelayStation01:
    py "<project>/Scripts/qa_relay_station.py"

Writes a machine-readable report to Saved/RelayStation01_QA.json.
This checks editor-world structure only; it does not prove navigation baking,
collision quality, successful interaction, AI behavior, or packaged runtime.
"""
import json
import os
import unreal

EXPECTED_MAP = "/Game/Maps/LVL_RelayStation01"
REPORT_NAME = "RelayStation01_QA.json"
GENERATED_PREFIX = "RS01_AUTO_"


def log(message):
    unreal.log("[RelayStation01 QA] " + str(message))


def run_qa():
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("No editor world is currently loaded.")

    package = world.get_outermost().get_name()
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    counts = {}
    generated_labels = []

    for actor in actors:
        class_name = actor.get_class().get_name()
        counts[class_name] = counts.get(class_name, 0) + 1
        try:
            label = actor.get_actor_label()
            if label.startswith(GENERATED_PREFIX):
                generated_labels.append(label)
        except Exception:
            pass

    checks = {
        "correct_map_open": package == EXPECTED_MAP,
        "player_start": counts.get("PlayerStart", 0) >= 1,
        "navmesh_bounds_volume": counts.get("NavMeshBoundsVolume", 0) >= 1,
        "three_signal_terminals": counts.get("SignalInteractable", 0) >= 3,
        "two_battery_pickups": counts.get("SignalBatteryPickup", 0) >= 2,
        "stalker_ai": counts.get("SignalStalker", 0) >= 1,
    }

    report = {
        "map": package,
        "expected_map": EXPECTED_MAP,
        "actor_count": len(actors),
        "class_counts": counts,
        "generated_actor_labels": sorted(generated_labels),
        "checks": checks,
        "passed": all(checks.values()),
        "scope": "Editor-world structural checks only; no gameplay, navmesh, build, or packaging test.",
    }

    report_path = os.path.join(unreal.Paths.project_saved_dir(), REPORT_NAME)
    os.makedirs(os.path.dirname(report_path), exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as handle:
        json.dump(report, handle, indent=2, sort_keys=True)
        handle.write("\\n")

    for name, passed in checks.items():
        log(("PASS" if passed else "FAIL") + ": " + name)
    log("Report: " + report_path)

    if not report["passed"]:
        failed = [name for name, passed in checks.items() if not passed]
        raise RuntimeError("Relay Station 01 QA failed: " + ", ".join(failed))

    log("PASS: all structural checks passed.")
    return report


if __name__ == "__main__":
    run_qa()
