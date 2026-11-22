#include "LastSignalCharacter.h"
#include "BatteryInventoryMath.h"
#include "SignalInteractable.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
ALastSignalCharacter::ALastSignalCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    bUseControllerRotationYaw = true;
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    Flashlight->SetupAttachment(FirstPersonCamera);
    Flashlight->SetRelativeLocation(FVector(18.0f, 8.0f, -6.0f));
    Flashlight->SetRelativeRotation(FRotator::ZeroRotator);
    Flashlight->SetIntensity(8500.0f);
    Flashlight->SetAttenuationRadius(1800.0f);
    Flashlight->SetInnerConeAngle(18.0f);
    Flashlight->SetOuterConeAngle(32.0f);
    Flashlight->SetVisibility(false);
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 260.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1200.0f;
}
void ALastSignalCharacter::BeginPlay()
{
    Super::BeginPlay();
    FlashlightBattery = FMath::Clamp(FlashlightBattery, 0.0f, 100.0f);
    Health = FMath::Clamp(MaxHealth, 1.0f, 100.0f);
}
void ALastSignalCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bFlashlightOn) return;
    FlashlightBattery = FMath::Max(0.0f, FlashlightBattery - FlashlightDrainPerSecond * DeltaSeconds);
    if (FlashlightBattery <= 0.0f)
    {
        bFlashlightOn = false;
        Flashlight->SetVisibility(false);
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Flashlight battery depleted"));
    }
}
float ALastSignalCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (bIsDead || AppliedDamage <= 0.0f) return 0.0f;
    Health = FMath::Max(0.0f, Health - AppliedDamage);
    if (Health <= 0.0f)
    {
        bIsDead = true;
        bFlashlightOn = false;
        if (Flashlight) Flashlight->SetVisibility(false);
        GetCharacterMovement()->DisableMovement();
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
        }
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("YOU DIED - THE SIGNAL IS LOST"));
    }
    else if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Red, FString::Printf(TEXT("INJURED  |  HEALTH %.0f%%"), Health));
    }
    return AppliedDamage;
}
void ALastSignalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ALastSignalCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ALastSignalCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ALastSignalCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ALastSignalCharacter::LookUp);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ALastSignalCharacter::TryInteract);
    PlayerInputComponent->BindAction(TEXT("Flashlight"), IE_Pressed, this, &ALastSignalCharacter::ToggleFlashlight);
    PlayerInputComponent->BindAction(TEXT("UseBattery"), IE_Pressed, this, &ALastSignalCharacter::UseBatteryPack);
}
void ALastSignalCharacter::MoveForward(float Value) { if (!bIsDead && Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorForwardVector(), Value); }
void ALastSignalCharacter::MoveRight(float Value) { if (!bIsDead && Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorRightVector(), Value); }
void ALastSignalCharacter::Turn(float Value) { if (!bIsDead) AddControllerYawInput(Value); }
void ALastSignalCharacter::LookUp(float Value) { if (!bIsDead) AddControllerPitchInput(Value); }
void ALastSignalCharacter::ToggleFlashlight()
{
    if (!Flashlight || bIsDead) return;
    if (bFlashlightOn) { bFlashlightOn = false; Flashlight->SetVisibility(false); return; }
    if (FlashlightBattery <= 0.0f)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("No flashlight battery"));
        return;
    }
    bFlashlightOn = true;
    Flashlight->SetVisibility(true);
}
bool ALastSignalCharacter::AddBatteryPack(int32 Amount)
{
    if (Amount <= 0 || bIsDead) return false;

    int32 UpdatedBatteryPacks = BatteryPacks;
    if (!LastSignal::TryAddBatteryPacks(BatteryPacks, Amount, UpdatedBatteryPacks))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Battery inventory full"));
        return false;
    }

    BatteryPacks = UpdatedBatteryPacks;
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Battery pack collected | Inventory: %d"), BatteryPacks));
    return true;
}
void ALastSignalCharacter::UseBatteryPack()
{
    if (bIsDead) return;
    if (BatteryPacks <= 0 || FlashlightBattery >= 100.0f)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, BatteryPacks <= 0 ? TEXT("No spare battery packs") : TEXT("Flashlight is already full"));
        return;
    }
    --BatteryPacks;
    FlashlightBattery = FMath::Min(100.0f, FlashlightBattery + BatteryPackCharge);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Flashlight recharged: %.0f%% | Packs: %d"), FlashlightBattery, BatteryPacks));
}
void ALastSignalCharacter::TryInteract()
{
    if (bIsDead || !Controller || !GetWorld() || !FirstPersonCamera) return;
    const FVector Start = FirstPersonCamera->GetComponentLocation();
    const FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionDistance;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LastSignalInteraction), false, this);
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
        if (ASignalInteractable* Target = Cast<ASignalInteractable>(Hit.GetActor())) Target->Interact(this);
}
