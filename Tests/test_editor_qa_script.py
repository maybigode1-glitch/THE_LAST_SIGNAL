"""Offline unit tests for the Unreal Editor structural-QA script.

A small Unreal Python API double exercises report/pass/fail behavior.
This does not simulate or certify Unreal Editor itself.
"""
import importlib.util
import json
import sys
import tempfile
import types
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "Scripts" / "qa_relay_station.py"
_FAKE_UNREAL = types.ModuleType("unreal")
_SPEC = importlib.util.spec_from_file_location("qa_relay_station_under_test", SCRIPT)
_MODULE = importlib.util.module_from_spec(_SPEC)
assert _SPEC and _SPEC.loader
_previous_unreal = sys.modules.get("unreal")
sys.modules["unreal"] = _FAKE_UNREAL
try:
    _SPEC.loader.exec_module(_MODULE)
finally:
    if _previous_unreal is None:
        sys.modules.pop("unreal", None)
    else:
        sys.modules["unreal"] = _previous_unreal


class _Name:
    def __init__(self, value):
        self.value = value

    def get_name(self):
        return self.value


class _Actor:
    def __init__(self, class_name, label="", class_error=None, label_error=None):
        self.class_name = class_name
        self.label = label
        self.class_error = class_error
        self.label_error = label_error

    def get_class(self):
        if self.class_error:
            raise RuntimeError(self.class_error)
        return _Name(self.class_name)

    def get_actor_label(self):
        if self.label_error:
            raise RuntimeError(self.label_error)
        return self.label


class _World:
    def __init__(self, package_name):
        self.package_name = package_name

    def get_outermost(self):
        return _Name(self.package_name)


class EditorQAScriptTests(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.world = _World("/Game/Maps/LVL_RelayStation01")
        self.actors = [
            _Actor("PlayerStart", "RS01_AUTO_PlayerStart_WestEntrance"),
            _Actor("NavMeshBoundsVolume", "RS01_AUTO_NavMeshBounds_RelayStation"),
            _Actor("SignalInteractable", "RS01_AUTO_TerminalA"),
            _Actor("SignalInteractable", "RS01_AUTO_TerminalB"),
            _Actor("SignalInteractable", "RS01_AUTO_PrimaryUplink"),
            _Actor("SignalBatteryPickup", "RS01_AUTO_BatteryA"),
            _Actor("SignalBatteryPickup", "RS01_AUTO_BatteryB"),
            _Actor("SignalStalker", "RS01_AUTO_Stalker_AI_NoAssignedMesh"),
        ]
        _FAKE_UNREAL.EditorLevelLibrary = types.SimpleNamespace(
            get_editor_world=lambda: self.world,
            get_all_level_actors=lambda: self.actors,
        )
        _FAKE_UNREAL.Paths = types.SimpleNamespace(
            project_saved_dir=lambda: self.temp_dir.name,
        )
        _FAKE_UNREAL.log = lambda _message: None

    def report_path(self):
        return Path(self.temp_dir.name) / "RelayStation01_QA.json"

    def test_expected_map_and_actor_set_produce_passing_json_report(self):
        report = _MODULE.run_qa()
        self.assertTrue(report["passed"])
        self.assertTrue(all(report["checks"].values()))
        self.assertEqual(report["actor_count"], 8)
        self.assertEqual(len(report["generated_actor_labels"]), 8)
        self.assertEqual(report["missing_generated_labels"], [])
        self.assertEqual(report["duplicate_generated_labels"], [])
        self.assertEqual(report["wrong_generated_actor_classes"], {})
        self.assertEqual(report["actor_inspection_errors"], [])
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertEqual(saved, report)

    def test_missing_required_actor_fails_and_still_writes_report(self):
        self.actors = [actor for actor in self.actors if actor.class_name != "SignalStalker"]
        with self.assertRaisesRegex(RuntimeError, "stalker_ai"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertFalse(saved["passed"])
        self.assertFalse(saved["checks"]["stalker_ai"])

    def test_wrong_map_is_reported_as_failure(self):
        self.world = _World("/Game/Maps/AnotherMap")
        with self.assertRaisesRegex(RuntimeError, "correct_map_open"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertFalse(saved["checks"]["correct_map_open"])

    def test_wrong_terminal_labels_fail_even_when_class_counts_match(self):
        self.actors[2] = _Actor("SignalInteractable", "RS01_AUTO_DecoyTerminal")
        with self.assertRaisesRegex(RuntimeError, "required_generated_actors"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertEqual(saved["class_counts"]["SignalInteractable"], 3)
        self.assertFalse(saved["checks"]["required_generated_actors"])
        self.assertEqual(saved["missing_generated_labels"], ["RS01_AUTO_TerminalA"])

    def test_duplicate_generated_label_fails_even_when_all_required_labels_exist(self):
        self.actors.append(_Actor("SignalInteractable", "RS01_AUTO_TerminalA"))
        with self.assertRaisesRegex(RuntimeError, "unique_generated_actor_labels"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertTrue(saved["checks"]["required_generated_actors"])
        self.assertFalse(saved["checks"]["unique_generated_actor_labels"])
        self.assertEqual(saved["duplicate_generated_labels"], ["RS01_AUTO_TerminalA"])

    def test_required_label_with_wrong_class_fails_even_when_class_counts_match(self):
        self.actors[2] = _Actor("StaticMeshActor", "RS01_AUTO_TerminalA")
        self.actors.append(_Actor("SignalInteractable", "RS01_AUTO_ExtraTerminal"))
        with self.assertRaisesRegex(RuntimeError, "required_generated_actor_classes"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertEqual(saved["class_counts"]["SignalInteractable"], 3)
        self.assertTrue(saved["checks"]["required_generated_actors"])
        self.assertFalse(saved["checks"]["required_generated_actor_classes"])
        self.assertEqual(
            saved["wrong_generated_actor_classes"],
            {
                "RS01_AUTO_TerminalA": {
                    "expected": "SignalInteractable",
                    "actual": ["StaticMeshActor"],
                }
            },
        )

    def test_unreadable_actor_label_fails_and_is_recorded_in_report(self):
        self.actors[0] = _Actor(
            "PlayerStart",
            "RS01_AUTO_PlayerStart_WestEntrance",
            label_error="label API unavailable",
        )
        with self.assertRaisesRegex(RuntimeError, "all_actors_inspectable"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertFalse(saved["checks"]["all_actors_inspectable"])
        self.assertEqual(
            saved["actor_inspection_errors"],
            [{
                "actor_index": 0,
                "stage": "label",
                "error": "RuntimeError: label API unavailable",
            }],
        )
        self.assertIn(
            "RS01_AUTO_PlayerStart_WestEntrance",
            saved["missing_generated_labels"],
        )

    def test_unreadable_actor_class_fails_and_is_recorded_in_report(self):
        self.actors[0] = _Actor(
            "PlayerStart",
            "RS01_AUTO_PlayerStart_WestEntrance",
            class_error="class API unavailable",
        )
        with self.assertRaisesRegex(RuntimeError, "all_actors_inspectable"):
            _MODULE.run_qa()
        saved = json.loads(self.report_path().read_text(encoding="utf-8"))
        self.assertFalse(saved["checks"]["all_actors_inspectable"])
        self.assertEqual(
            saved["actor_inspection_errors"],
            [{
                "actor_index": 0,
                "stage": "class",
                "error": "RuntimeError: class API unavailable",
            }],
        )
        self.assertNotIn("PlayerStart", saved["class_counts"])


if __name__ == "__main__":
    unittest.main()
