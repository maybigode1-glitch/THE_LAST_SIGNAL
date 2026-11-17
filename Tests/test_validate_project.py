import importlib.util
import json
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "Scripts" / "validate_project.py"
SPEC = importlib.util.spec_from_file_location("validate_project", SCRIPT)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC and SPEC.loader
SPEC.loader.exec_module(MODULE)

class ValidateProjectTests(unittest.TestCase):
    def test_valid_project_metadata(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "sample.uproject"
            path.write_text(json.dumps({"EngineAssociation": "5.4", "Modules": [{"Name": "TheLastSignal", "Type": "Runtime"}], "Plugins": [{"Name": "PythonScriptPlugin", "Enabled": True}, {"Name": "EditorScriptingUtilities", "Enabled": True}]}), encoding="utf-8")
            self.assertEqual(MODULE.validate_uproject(path), [])

    def test_missing_engine_association_is_reported(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "sample.uproject"
            path.write_text(json.dumps({"Modules": []}), encoding="utf-8")
            self.assertTrue(any("EngineAssociation" in e for e in MODULE.validate_uproject(path)))

    def test_invalid_json_is_reported(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "sample.uproject"
            path.write_text("{not-json", encoding="utf-8")
            self.assertTrue(MODULE.validate_uproject(path))

if __name__ == "__main__":
    unittest.main()
