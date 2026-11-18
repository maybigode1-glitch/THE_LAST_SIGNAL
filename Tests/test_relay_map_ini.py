"""Offline tests for the Relay Station map builder's INI update helper.

These tests exercise only standard-library file editing. They do not import or
simulate Unreal Editor behavior beyond providing the module's import dependency.
"""
import importlib.util
import sys
import tempfile
import types
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "Scripts" / "create_relay_station_map.py"

_FAKE_UNREAL = types.ModuleType("unreal")
_FAKE_UNREAL.Paths = types.SimpleNamespace(project_dir=lambda: str(ROOT))
_SPEC = importlib.util.spec_from_file_location("create_relay_station_map_under_test", SCRIPT)
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


class UpdateIniSectionTests(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.ini_path = Path(self.temp_dir.name) / "DefaultEngine.ini"

    def test_updates_target_keys_and_preserves_other_settings_and_sections(self):
        self.ini_path.write_text(
            "[/Script/EngineSettings.GameMapsSettings]\n"
            "GameDefaultMap=/Game/OldMap.OldMap\n"
            "UnrelatedSetting=True\n"
            "\n"
            "[/Script/Other.Section]\n"
            "OtherValue=PreserveMe\n",
            encoding="utf-8",
        )

        _MODULE.update_ini_section(
            str(self.ini_path),
            "/Script/EngineSettings.GameMapsSettings",
            {
                "GameDefaultMap": "/Game/Maps/Relay.Relay",
                "EditorStartupMap": "/Game/Maps/Relay.Relay",
            },
        )

        result = self.ini_path.read_text(encoding="utf-8")
        target_section = result.split("[/Script/Other.Section]", 1)[0]
        self.assertEqual(target_section.count("GameDefaultMap="), 1)
        self.assertIn("GameDefaultMap=/Game/Maps/Relay.Relay", target_section)
        self.assertIn("EditorStartupMap=/Game/Maps/Relay.Relay", target_section)
        self.assertIn("UnrelatedSetting=True", target_section)
        self.assertIn("OtherValue=PreserveMe", result)

    def test_creates_missing_section_without_discarding_existing_content(self):
        self.ini_path.write_text(
            "[/Script/Existing.Section]\nKeep=1\n",
            encoding="utf-8",
        )

        _MODULE.update_ini_section(
            str(self.ini_path),
            "/Script/NavigationSystem.RecastNavMesh",
            {"RuntimeGeneration": "Dynamic"},
        )

        result = self.ini_path.read_text(encoding="utf-8")
        self.assertIn("[/Script/Existing.Section]\nKeep=1", result)
        self.assertIn(
            "[/Script/NavigationSystem.RecastNavMesh]\nRuntimeGeneration=Dynamic",
            result,
        )

    def test_repeating_same_update_is_idempotent(self):
        self.ini_path.write_text(
            "[/Script/NavigationSystem.RecastNavMesh]\n"
            "RuntimeGeneration=Static\n",
            encoding="utf-8",
        )
        values = {"RuntimeGeneration": "Dynamic", "AgentMaxStepHeight": "45.0"}

        _MODULE.update_ini_section(
            str(self.ini_path), "/Script/NavigationSystem.RecastNavMesh", values
        )
        first_result = self.ini_path.read_text(encoding="utf-8")
        _MODULE.update_ini_section(
            str(self.ini_path), "/Script/NavigationSystem.RecastNavMesh", values
        )
        second_result = self.ini_path.read_text(encoding="utf-8")

        self.assertEqual(second_result, first_result)
        self.assertEqual(second_result.count("RuntimeGeneration="), 1)
        self.assertEqual(second_result.count("AgentMaxStepHeight="), 1)


if __name__ == "__main__":
    unittest.main()
