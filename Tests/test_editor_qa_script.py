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
    def __init__(self, class_name, label=""):
        self.class_name = class_name
        self.label = label

    def get_class(self):
        return _Name(self.class_name)

    def get_actor_label(self):
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
            _Actor("PlayerStart", "RS01_AUTO_PlayerStart"),
            _Actor("NavMeshBoundsVolume", "RS01_AUTO_NavMeshBounds"),
            *[_Actor("SignalInteractable", f"RS01_AUTO_Terminal{i}") for i in range(3)],
            *[_Actor("SignalBatteryPickup", f"RS01_AUTO_Battery{i}") for i in range(2)],
            _Actor("SignalStalker", "RS01_AUTO_Stalker"),
        ]
        _FAKE_UNREAL.EditorLevelLibrary = types.SimpleNamespace(
            get_editor_world=lambda: self.world,
            get_all_level_actors=lambda: self.actors,
        )
        _FAKE_UNREAL.Paths = types.SimpleNamespace(
            project_saved_dir=lambda: self.temp_dir.name,
        )
        _FAKE_UNREAL.log = lambda _message: None

    def test_expected_map_and_actor_set_produce_passing_json_report(self):
        report = _MODULE.run_qa()
        self.assertTrue(report["passed"])
        self.assertTrue(all(report["checks"].values()))
        self.assertEqual(report["actor_count"], 8)
        self.assertEqual(len(report["generated_actor_labels"]), 8)
        report_path = Path(self.temp_dir.name) / "RelayStation01_QA.json"
        self.assertTrue(report_path.is_file())
        saved = json.loads(report_path.read_text(encoding="utf-8"))
        self.assertEqual(saved, report)

    def test_missing_required_actor_fails_and_still_writes_report(self):
        self.actors = [actor for actor in self.actors if actor.class_name != "SignalStalker"]
        with self.assertRaisesRegex(RuntimeError, "stalker_ai"):
            _MODULE.run_qa()
        report_path = Path(self.temp_dir.name) / "RelayStation01_QA.json"
        saved = json.loads(report_path.read_text(encoding="utf-8"))
        self.assertFalse(saved["passed"])
        self.assertFalse(saved["checks"]["stalker_ai"])

    def test_wrong_map_is_reported_as_failure(self):
        self.world = _World("/Game/Maps/AnotherMap")
        with self.assertRaisesRegex(RuntimeError, "correct_map_open"):
            _MODULE.run_qa()
        report_path = Path(self.temp_dir.name) / "RelayStation01_QA.json"
        saved = json.loads(report_path.read_text(encoding="utf-8"))
        self.assertFalse(saved["checks"]["correct_map_open"])


if __name__ == "__main__":
    unittest.main()
