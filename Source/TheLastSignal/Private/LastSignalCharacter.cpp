#include "LastSignalCharacter.h"
#include "SignalInteractable.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

ALastSignalCharacter::ALastSignalCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    bUseControllerRotationYaw = true;
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 260.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1200.0f;
}

void ALastSignalCharacter::BeginPlay()
{
    Super::BeginPlay();
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
}

void ALastSignalCharacter::MoveForward(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorForwardVector(), Value);
}
void ALastSignalCharacter::MoveRight(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorRightVector(), Value);
}
void ALastSignalCharacter::Turn(float Value) { AddControllerYawInput(Value); }
void ALastSignalCharacter::LookUp(float Value) { AddControllerPitchInput(Value); }

void ALastSignalCharacter::TryInteract()
{
    if (!Controller || !GetWorld() || !FirstPersonCamera) return;
    const FVector Start = FirstPersonCamera->GetComponentLocation();
    const FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionDistance;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LastSignalInteraction), false, this);
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
    {
        if (ASignalInteractable* Target = Cast<ASignalInteractable>(Hit.GetActor()))
        {
            Target->Interact(this);
        }
    }
}
