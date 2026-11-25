"""Offline regression tests for safe Relay Station map replacement.

These tests use a narrow Unreal Python API double. They do not execute Unreal Editor.
"""
import ast
import importlib.util
import sys
import tempfile
import types
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILDER_PATH = ROOT / "Scripts" / "create_relay_station_map.py"


def load_builder(fake_unreal):
    spec = importlib.util.spec_from_file_location(
        "create_relay_station_map_under_test", BUILDER_PATH
    )
    module = importlib.util.module_from_spec(spec)
    assert spec and spec.loader
    previous_unreal = sys.modules.get("unreal")
    sys.modules["unreal"] = fake_unreal
    try:
        spec.loader.exec_module(module)
    finally:
        if previous_unreal is None:
            sys.modules.pop("unreal", None)
        else:
            sys.modules["unreal"] = previous_unreal
    return module


def make_fake_unreal(events, missing_class=None):
    fake = types.ModuleType("unreal")
    fake.Paths = types.SimpleNamespace(project_dir=lambda: tempfile.gettempdir())
    fake.StaticMeshActor = object()
    fake.PlayerStart = object()
    fake.load_object = lambda *_args: object()

    def load_class(_outer, path):
        if missing_class and path.endswith("." + missing_class):
            return None
        return object()

    fake.load_class = load_class
    fake.log = lambda _message: None
    fake.EditorAssetLibrary = types.SimpleNamespace(
        make_directory=lambda path: events.append(("make_directory", path)),
        does_asset_exist=lambda path: events.append(("does_asset_exist", path)) or True,
    )
    fake.EditorLevelLibrary = types.SimpleNamespace(
        load_level=lambda path: events.append(("load_level", path)) or True,
        new_level=lambda path: events.append(("new_level", path)) or True,
        get_all_level_actors=lambda: events.append(("get_all_level_actors",)) or [],
        destroy_actor=lambda _actor: events.append(("destroy_actor",)) or True,
    )
    return fake


class _Actor:
    def __init__(self, label):
        self.label = label

    def get_actor_label(self):
        return self.label


class RelayStationBuilderPreflightTests(unittest.TestCase):
    def test_missing_cube_aborts_before_any_level_operation(self):
        events = []
        fake = make_fake_unreal(events)
        fake.load_object = lambda *_args: None
        builder = load_builder(fake)

        with self.assertRaisesRegex(RuntimeError, "BasicShapes/Cube"):
            builder.build_level()

        self.assertEqual(events, [])

    def test_missing_cpp_class_aborts_before_existing_level_is_opened_or_cleared(self):
        events = []
        fake = make_fake_unreal(events, missing_class="SignalStalker")
        builder = load_builder(fake)

        with self.assertRaisesRegex(RuntimeError, "SignalStalker"):
            builder.build_level()

        self.assertEqual(events, [])

    def test_failed_generated_actor_deletion_is_reported(self):
        events = []
        generated = _Actor("RS01_AUTO_TerminalA")
        unrelated = _Actor("DesignerPlaced_Light")
        fake = make_fake_unreal(events)
        fake.EditorLevelLibrary.get_all_level_actors = lambda: [generated, unrelated]
        fake.EditorLevelLibrary.destroy_actor = (
            lambda actor: events.append(actor.get_actor_label()) or False
        )
        builder = load_builder(fake)

        with self.assertRaisesRegex(RuntimeError, "RS01_AUTO_TerminalA"):
            builder.destroy_previous_generated_actors()

        self.assertEqual(events, ["RS01_AUTO_TerminalA"])

    def test_project_startup_settings_are_written_only_after_level_and_asset_save(self):
        tree = ast.parse(BUILDER_PATH.read_text(encoding="utf-8"))
        build_level = next(
            node for node in tree.body
            if isinstance(node, ast.FunctionDef) and node.name == "build_level"
        )
        call_lines = {}
        for node in ast.walk(build_level):
            if not isinstance(node, ast.Call):
                continue
            function = node.func
            name = function.attr if isinstance(function, ast.Attribute) else (
                function.id if isinstance(function, ast.Name) else None
            )
            if name in {"save_current_level", "save_asset", "update_ini_section"}:
                call_lines.setdefault(name, []).append(node.lineno)

        level_save = min(call_lines["save_current_level"])
        asset_save = min(call_lines["save_asset"])
        config_updates = call_lines["update_ini_section"]
        self.assertLess(level_save, asset_save)
        self.assertTrue(config_updates)
        self.assertTrue(all(line > asset_save for line in config_updates))


if __name__ == "__main__":
    unittest.main()
