import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class ObjectiveProgressContractTests(unittest.TestCase):
    def test_game_mode_uses_shared_objective_progress_rules(self):
        header = (ROOT / "Source/TheLastSignal/Public/LastSignalGameMode.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalGameMode.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "ObjectiveProgressMath.h"', header)
        self.assertIn("LastSignal::ClampSignalsCollected(SignalsCollected, RequiredSignals)", header)
        self.assertIn("LastSignal::NormalizeRequiredSignals(RequiredSignals)", header)
        self.assertIn("LastSignal::AreSignalObjectivesComplete(SignalsCollected, RequiredSignals)", header)
        self.assertIn("LastSignal::TryRegisterSignalCollected(", source)

    def test_game_mode_normalizes_progress_before_returning_on_completed_objectives(self):
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalGameMode.cpp").read_text(encoding="utf-8")
        registration = source.split("void ALastSignalGameMode::RegisterSignalCollected()", 1)[1]
        self.assertLess(
            registration.index("TryRegisterSignalCollected"),
            registration.index("SignalsCollected = UpdatedSignalsCollected;")
        )
        self.assertLess(
            registration.index("SignalsCollected = UpdatedSignalsCollected;"),
            registration.index("if (!bAccepted) return;")
        )
        self.assertIn("GetRequiredSignals()", registration)


if __name__ == "__main__":
    unittest.main()
