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
REQUIRED_GENERATED_ACTOR_CLASSES = {
    "RS01_AUTO_PlayerStart_WestEntrance": "PlayerStart",
    "RS01_AUTO_NavMeshBounds_RelayStation": "NavMeshBoundsVolume",
    "RS01_AUTO_TerminalA": "SignalInteractable",
    "RS01_AUTO_TerminalB": "SignalInteractable",
    "RS01_AUTO_PrimaryUplink": "SignalInteractable",
    "RS01_AUTO_BatteryA": "SignalBatteryPickup",
    "RS01_AUTO_BatteryB": "SignalBatteryPickup",
    "RS01_AUTO_Stalker_AI_NoAssignedMesh": "SignalStalker",
}
REQUIRED_GENERATED_LABELS = set(REQUIRED_GENERATED_ACTOR_CLASSES)


def log(message):
    unreal.log("[RelayStation01 QA] " + str(message))


def run_qa():
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("No editor world is currently loaded.")

    package = world.get_outermost().get_name()
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    counts = {}
    generated_actor_classes = {}
    inspection_errors = []

    for actor_index, actor in enumerate(actors):
        try:
            class_name = actor.get_class().get_name()
        except Exception as exc:
            inspection_errors.append({
                "actor_index": actor_index,
                "stage": "class",
                "error": type(exc).__name__ + ": " + str(exc),
            })
            continue

        counts[class_name] = counts.get(class_name, 0) + 1

        try:
            label = actor.get_actor_label()
            if not isinstance(label, str):
                raise TypeError("get_actor_label() did not return a string")
        except Exception as exc:
            inspection_errors.append({
                "actor_index": actor_index,
                "stage": "label",
                "error": type(exc).__name__ + ": " + str(exc),
            })
            continue

        if label.startswith(GENERATED_PREFIX):
            generated_actor_classes.setdefault(label, []).append(class_name)

    generated_labels = sorted(
        label
        for label, class_names in generated_actor_classes.items()
        for _ in class_names
    )
    generated_label_set = set(generated_actor_classes)
    missing_generated_labels = sorted(
        REQUIRED_GENERATED_LABELS - generated_label_set
    )
    duplicate_generated_labels = sorted(
        label
        for label, class_names in generated_actor_classes.items()
        if len(class_names) > 1
    )
    wrong_generated_actor_classes = {
        label: {
            "expected": expected_class,
            "actual": sorted(set(generated_actor_classes.get(label, []))),
        }
        for label, expected_class in REQUIRED_GENERATED_ACTOR_CLASSES.items()
        if label in generated_actor_classes
        and any(
            class_name != expected_class
            for class_name in generated_actor_classes[label]
        )
    }
    checks = {
        "correct_map_open": package == EXPECTED_MAP,
        "all_actors_inspectable": not inspection_errors,
        "player_start": counts.get("PlayerStart", 0) >= 1,
        "navmesh_bounds_volume": counts.get("NavMeshBoundsVolume", 0) >= 1,
        "three_signal_terminals": counts.get("SignalInteractable", 0) >= 3,
        "two_battery_pickups": counts.get("SignalBatteryPickup", 0) >= 2,
        "stalker_ai": counts.get("SignalStalker", 0) >= 1,
        "required_generated_actors": not missing_generated_labels,
        "unique_generated_actor_labels": not duplicate_generated_labels,
        "required_generated_actor_classes": not wrong_generated_actor_classes,
    }

    report = {
        "map": package,
        "expected_map": EXPECTED_MAP,
        "actor_count": len(actors),
        "class_counts": counts,
        "actor_inspection_errors": inspection_errors,
        "generated_actor_labels": generated_labels,
        "generated_actor_classes": {
            label: sorted(class_names)
            for label, class_names in sorted(generated_actor_classes.items())
        },
        "required_generated_labels": sorted(REQUIRED_GENERATED_LABELS),
        "missing_generated_labels": missing_generated_labels,
        "duplicate_generated_labels": duplicate_generated_labels,
        "wrong_generated_actor_classes": wrong_generated_actor_classes,
        "checks": checks,
        "passed": all(checks.values()),
        "scope": "Editor-world structural checks only; no gameplay, navmesh, build, or packaging test.",
    }

    report_path = os.path.join(unreal.Paths.project_saved_dir(), REPORT_NAME)
    os.makedirs(os.path.dirname(report_path), exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as handle:
        json.dump(report, handle, indent=2, sort_keys=True)
        handle.write("\n")

    for name, passed in checks.items():
        log(("PASS" if passed else "FAIL") + ": " + name)
    if inspection_errors:
        log("Actor inspection errors: " + json.dumps(inspection_errors, sort_keys=True))
    if missing_generated_labels:
        log("Missing generated actors: " + ", ".join(missing_generated_labels))
    if duplicate_generated_labels:
        log("Duplicate generated actor labels: " + ", ".join(duplicate_generated_labels))
    if wrong_generated_actor_classes:
        log("Generated actors with unexpected classes: " + json.dumps(
            wrong_generated_actor_classes, sort_keys=True
        ))
    log("Report: " + report_path)

    if not report["passed"]:
        failed = [name for name, passed in checks.items() if not passed]
        raise RuntimeError("Relay Station 01 QA failed: " + ", ".join(failed))

    log("PASS: all structural checks passed.")
    return report


if __name__ == "__main__":
    run_qa()
