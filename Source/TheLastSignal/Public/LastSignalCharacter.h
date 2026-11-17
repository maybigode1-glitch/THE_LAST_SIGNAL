#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LastSignalCharacter.generated.h"
class UCameraComponent;
class USpotLightComponent;
UCLASS()
class THELASTSIGNAL_API ALastSignalCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ALastSignalCharacter();
    UFUNCTION(BlueprintPure, Category = "Flashlight") float GetFlashlightBatteryPercent() const { return FlashlightBattery; }
    UFUNCTION(BlueprintPure, Category = "Flashlight") bool IsFlashlightOn() const { return bFlashlightOn; }
    UFUNCTION(BlueprintPure, Category = "Inventory") int32 GetBatteryPackCount() const { return BatteryPacks; }
    UFUNCTION(BlueprintCallable, Category = "Inventory") void AddBatteryPack(int32 Amount = 1);
    UFUNCTION(BlueprintCallable, Category = "Inventory") bool UseBatteryPack();
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void TryInteract();
    void ToggleFlashlight();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components") TObjectPtr<UCameraComponent> FirstPersonCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components") TObjectPtr<USpotLightComponent> Flashlight;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "50.0")) float InteractionDistance = 275.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight", meta = (ClampMin = "0.0")) float FlashlightDrainPerSecond = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1.0", ClampMax = "100.0")) float BatteryPackCharge = 35.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flashlight", meta = (ClampMin = "0.0", ClampMax = "100.0")) float FlashlightBattery = 100.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flashlight") bool bFlashlightOn = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0")) int32 BatteryPacks = 0;
};
