import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "Scripts" / "validate_project.py"
SPEC = importlib.util.spec_from_file_location("validate_project", SCRIPT)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC and SPEC.loader
SPEC.loader.exec_module(MODULE)

class RepositoryContractTests(unittest.TestCase):
    def test_required_ue5_files_exist(self):
        self.assertEqual(MODULE.validate_repository(ROOT), [])

    def test_project_declares_runtime_module(self):
        self.assertEqual(MODULE.validate_uproject(ROOT / "THE_LAST_SIGNAL.uproject"), [])

    def test_flashlight_is_bound_and_has_battery_logic(self):
        input_config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
        character_source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('ActionName="Flashlight",Key=F', input_config)
        self.assertIn("USpotLightComponent", character_source)
        self.assertIn("FlashlightDrainPerSecond", character_source)
        self.assertIn("FlashlightBattery <= 0.0f", character_source)

    def test_missing_source_file_is_reported(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "THE_LAST_SIGNAL.uproject").write_text(
                '{"EngineAssociation":"5.4","Modules":[{"Name":"TheLastSignal"}]}',
                encoding="utf-8"
            )
            errors = MODULE.validate_repository(root)
            self.assertTrue(any("LastSignalCharacter.cpp" in error for error in errors))

if __name__ == "__main__":
    unittest.main()
