import importlib.util
import json
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
    def test_project_declares_runtime_module_and_editor_automation_plugins(self):
        project = json.loads((ROOT / "THE_LAST_SIGNAL.uproject").read_text(encoding="utf-8"))
        self.assertEqual(MODULE.validate_uproject(ROOT / "THE_LAST_SIGNAL.uproject"), [])
        plugins = {p["Name"]: p["Enabled"] for p in project["Plugins"]}
        self.assertTrue(plugins["PythonScriptPlugin"])
        self.assertTrue(plugins["EditorScriptingUtilities"])
    def test_editor_map_script_creates_and_configures_level(self):
        source = (ROOT / "Scripts/create_relay_station_map.py").read_text(encoding="utf-8")
        for token in ("LVL_RelayStation01", "PlayerStart", "NavMeshBoundsVolume", "GameDefaultMap", "EditorStartupMap", "save_current_level", "RuntimeGeneration", "Dynamic", "SignalBatteryPickup", "SignalInteractable", "SignalStalker"):
            self.assertIn(token, source)
    def test_flashlight_and_inventory_input_bindings(self):
        config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('ActionName="Flashlight",Key=F', config)
        self.assertIn('ActionName="UseBattery",Key=R', config)
        for token in ("USpotLightComponent", "FlashlightDrainPerSecond", "AddBatteryPack", "UseBatteryPack"):
            self.assertIn(token, source)
    def test_flashlight_battery_uses_finite_safe_shared_helper(self):
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "FlashlightBatteryMath.h"', source)
        self.assertIn("LastSignal::ClampFlashlightBattery(FlashlightBattery)", source)
        self.assertIn(
            "LastSignal::DrainFlashlightBattery(FlashlightBattery, FlashlightDrainPerSecond, DeltaSeconds)",
            source
        )
        self.assertIn("LastSignal::TryRechargeFlashlightBattery(", source)
        self.assertIn("BatteryPackCharge, UpdatedFlashlightBattery", source)
        self.assertNotIn("FlashlightBattery - FlashlightDrainPerSecond * DeltaSeconds", source)
        self.assertNotIn("FlashlightBattery + BatteryPackCharge", source)
    def test_flashlight_pack_is_consumed_only_after_successful_recharge(self):
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        use_battery = source.split("void ALastSignalCharacter::UseBatteryPack()", 1)[1].split(
            "\nvoid ALastSignalCharacter::TryInteract()", 1
        )[0]
        self.assertIn("if (!LastSignal::TryRechargeFlashlightBattery(", use_battery)
        self.assertLess(
            use_battery.index("TryRechargeFlashlightBattery"),
            use_battery.index("--BatteryPacks;")
        )
        failure_path = use_battery.split("if (!LastSignal::TryRechargeFlashlightBattery(", 1)[1].split(
            "--BatteryPacks;", 1
        )[0]
        self.assertIn("return;", failure_path)
        self.assertIn("FlashlightBattery = UpdatedFlashlightBattery;", use_battery)
        self.assertIn("bFlashlightOn = false;", source)

    def test_battery_inventory_uses_overflow_safe_shared_helper(self):
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "BatteryInventoryMath.h"', source)
        self.assertIn("LastSignal::TryAddBatteryPacks(BatteryPacks, Amount, UpdatedBatteryPacks)", source)
        self.assertNotIn("FMath::Clamp(BatteryPacks + Amount", source)
    def test_battery_pickup_is_consumed_only_after_inventory_accepts_pack(self):
        pickup = (ROOT / "Source/TheLastSignal/Private/SignalBatteryPickup.cpp").read_text(encoding="utf-8")
        character_header = (ROOT / "Source/TheLastSignal/Public/LastSignalCharacter.h").read_text(encoding="utf-8")
        character = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn("bool AddBatteryPack", character_header)
        self.assertIn("LastSignal::TryAddBatteryPacks", character)
        interaction = pickup.split("void ASignalBatteryPickup::Interact_Implementation", 1)[1]
        self.assertIn("if (!Character->AddBatteryPack(PacksGranted)) return;", interaction)
        self.assertLess(interaction.index("AddBatteryPack"), interaction.index("Destroy()"))
        self.assertIn("bHasBeenActivated = true;", interaction)
    def test_signal_interactions_update_objective_progress(self):
        interactable = (ROOT / "Source/TheLastSignal/Private/SignalInteractable.cpp").read_text(encoding="utf-8")
        game_mode = (ROOT / "Source/TheLastSignal/Private/LastSignalGameMode.cpp").read_text(encoding="utf-8")
        self.assertIn("RegisterSignalCollected", interactable)
        self.assertIn("AreObjectivesComplete", game_mode)
        self.assertIn("Cast<ALastSignalCharacter>(InteractingActor)", interactable)
        self.assertIn("if (!Mode || Mode->AreObjectivesComplete()) return;", interactable)
    def test_survival_hud_displays_battery_inventory_and_objectives(self):
        hud = (ROOT / "Source/TheLastSignal/Private/LastSignalHUD.cpp").read_text(encoding="utf-8")
        mode = (ROOT / "Source/TheLastSignal/Private/LastSignalGameMode.cpp").read_text(encoding="utf-8")
        self.assertIn("GetFlashlightBatteryPercent", hud)
        self.assertIn("GetBatteryPackCount", hud)
        self.assertIn("GetSignalsCollected", hud)
        self.assertIn("GetRequiredSignals", hud)
        self.assertIn("ALastSignalHUD::StaticClass", mode)
        self.assertNotIn("\n        const float HealthY", hud)
    def test_stalker_ai_has_patrol_chase_search_and_attack(self):
        source = (ROOT / "Source/TheLastSignal/Private/SignalStalker.cpp").read_text(encoding="utf-8")
        for token in ("LineOfSightTo", "MoveToActor", "MoveToLocation", "PerformAttack", "GetRandomReachablePointInRadius"):
            self.assertIn(token, source)
        self.assertIn("AIModule", (ROOT / "Source/TheLastSignal/TheLastSignal.Build.cs").read_text(encoding="utf-8"))
    def test_stalker_attack_defers_damage_and_revalidates_at_impact(self):
        source = (ROOT / "Source/TheLastSignal/Private/SignalStalker.cpp").read_text(encoding="utf-8")
        attack = source.split("void ASignalStalker::PerformAttack()", 1)[1].split(
            "void ASignalStalker::ApplyAttackDamage()", 1
        )[0]
        impact = source.split("void ASignalStalker::ApplyAttackDamage()", 1)[1].split(
            "void ASignalStalker::ClearAttackState()", 1
        )[0]
        self.assertIn("&ASignalStalker::ApplyAttackDamage", attack)
        self.assertIn("AttackImpactDelay", attack)
        self.assertIn("AttackMontage->GetPlayLength() * 0.35f", attack)
        self.assertIn(": 0.3f", attack)
        self.assertNotIn("TakeDamage", attack)
        self.assertIn('#include "StalkerMeleeRules.h"', source)
        self.assertIn("const bool bHasLineOfSight = bHasAIController && AI->LineOfSightTo(Player);", impact)
        self.assertIn(
            "LastSignal::CanApplyStalkerMeleeDamage(Distance, AttackRange, bIsAttacking, bHasAIController, bHasLineOfSight)",
            impact
        )
        self.assertLess(impact.index("CanApplyStalkerMeleeDamage"), impact.index("Player->TakeDamage"))
        self.assertIn("Player->TakeDamage", impact)
    def test_player_health_and_death_are_connected_to_hud(self):
        character = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        hud = (ROOT / "Source/TheLastSignal/Private/LastSignalHUD.cpp").read_text(encoding="utf-8")
        self.assertIn("TakeDamage", character)
        self.assertIn("bIsDead", character)
        self.assertIn("GetHealthPercent", hud)
        self.assertIn("HEALTH", hud)
    def test_player_rejects_invalid_damage_before_super_and_uses_shared_health_math(self):
        source = (ROOT / "Source/TheLastSignal/Private/LastSignalCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "SurvivalHealthMath.h"', source)
        damage = source.split("float ALastSignalCharacter::TakeDamage", 1)[1].split(
            "\nvoid ALastSignalCharacter::SetupPlayerInputComponent", 1
        )[0]
        self.assertLess(
            damage.index("bIsDead || !LastSignal::IsValidDamageAmount(DamageAmount)"),
            damage.index("Super::TakeDamage")
        )
        self.assertIn(
            "TryApplyDamageToHealth(Health, DamageAfterEngineRules, UpdatedHealth, ActualDamage)",
            damage
        )
        self.assertLess(
            damage.index("TryApplyDamageToHealth"),
            damage.index("Health = UpdatedHealth")
        )
        self.assertIn("return ActualDamage;", damage)
        self.assertNotIn("return DamageAfterEngineRules;", damage)
    def test_stalker_animation_state_and_montage_hooks(self):
        header = (ROOT / "Source/TheLastSignal/Public/SignalStalkerAnimInstance.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/SignalStalkerAnimInstance.cpp").read_text(encoding="utf-8")
        enemy = (ROOT / "Source/TheLastSignal/Private/SignalStalker.cpp").read_text(encoding="utf-8")
        for token in ("GroundSpeed", "bIsMoving", "bIsChasing", "bIsSearching", "bIsAttacking", "bIsDead"):
            self.assertIn(token, header)
        for token in ("GroundSpeed", "IsChasing", "IsSearching", "IsAttacking", "IsDead"):
            self.assertIn(token, source)
        self.assertIn("Montage_Play", enemy)
        self.assertIn("ApplyAttackDamage", enemy)
        self.assertIn("AttackImpactTimer", enemy)
        self.assertIn("StalkerMeleeRangeGrace = 75.0f", (ROOT / "Source/TheLastSignal/Public/StalkerMeleeRules.h").read_text(encoding="utf-8"))
        self.assertIn("GreyboxVisual", enemy)
        self.assertIn("BasicShapes/Cube.Cube", enemy)
    def test_stalker_audio_cues_are_configurable_and_triggered(self):
        header = (ROOT / "Source/TheLastSignal/Public/SignalStalker.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/SignalStalker.cpp").read_text(encoding="utf-8")
        for cue in ("FootstepSound", "AlertSound", "AttackSound", "SearchSound", "AmbientLoopSound", "ChaseLoopSound"):
            self.assertIn(cue, header)
            self.assertIn(cue, source)
        self.assertIn("PlaySoundAtLocation", source)
    def test_first_area_builder_creates_route_objectives_pickups_and_enemy(self):
        header = (ROOT / "Source/TheLastSignal/Public/SignalPrototypeArea.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/TheLastSignal/Private/SignalPrototypeArea.cpp").read_text(encoding="utf-8")
        guide = (ROOT / "Docs/FIRST_PLAYABLE_AREA.md").read_text(encoding="utf-8")
        for token in ("BuildGeometry", "SpawnObjectivesAndPickups", "SpawnStalker", "bSpawnStalker", "bSpawnPickups"):
            self.assertIn(token, header + source)
        for token in ("Relay Terminal A", "Relay Terminal B", "Primary Signal Uplink", "Battery Pack", "NavMeshBoundsVolume"):
            self.assertIn(token, source + guide)
    def test_editor_qa_script_checks_required_relay_station_actors(self):
        source = (ROOT / "Scripts/qa_relay_station.py").read_text(encoding="utf-8")
        for token in ("get_editor_world", "PlayerStart", "NavMeshBoundsVolume", "SignalInteractable", "SignalBatteryPickup", "SignalStalker", "RelayStation01_QA.json", "all(checks.values())"):
            self.assertIn(token, source)
        compile(source, "qa_relay_station.py", "exec")
    def test_missing_source_file_is_reported(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "THE_LAST_SIGNAL.uproject").write_text(
                '{"EngineAssociation":"5.4","Modules":[{"Name":"TheLastSignal"}],"Plugins":[{"Name":"PythonScriptPlugin","Enabled":true},{"Name":"EditorScriptingUtilities","Enabled":true}]}', encoding="utf-8")
            errors = MODULE.validate_repository(root)
            self.assertTrue(any("LastSignalCharacter.cpp" in error for error in errors))
if __name__ == "__main__":
    unittest.main()
