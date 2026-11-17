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
    def test_flashlight_and_inventory_input_bindings(self):
        config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('ActionName="Flashlight",Key=F', config)
        self.assertIn('ActionName="UseBattery",Key=R', config)
        for token in ("USpotLightComponent", "FlashlightDrainPerSecond", "AddBatteryPack", "UseBatteryPack"):
            self.assertIn(token, source)
    def test_battery_pickup_adds_inventory_and_is_consumed(self):
        source = (ROOT / "Source/TheLastSignal/Private/SignalBatteryPickup.cpp").read_text(encoding="utf-8")
        self.assertIn("AddBatteryPack", source)
        self.assertIn("Destroy()", source)
    def test_signal_interactions_update_objective_progress(self):
        interactable = (ROOT / "Source/TheLastSignal/Private/SignalInteractable.cpp").read_text(encoding="utf-8")
        game_mode = (ROOT / "Source/TheLastSignal/Private/LastSignalGameMode.cpp").read_text(encoding="utf-8")
        self.assertIn("RegisterSignalCollected", interactable)
        self.assertIn("AreObjectivesComplete", game_mode)
    def test_missing_source_file_is_reported(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "THE_LAST_SIGNAL.uproject").write_text(
                '{"EngineAssociation":"5.4","Modules":[{"Name":"TheLastSignal"}]}', encoding="utf-8")
            errors = MODULE.validate_repository(root)
            self.assertTrue(any("LastSignalCharacter.cpp" in error for error in errors))
if __name__ == "__main__":
    unittest.main()
