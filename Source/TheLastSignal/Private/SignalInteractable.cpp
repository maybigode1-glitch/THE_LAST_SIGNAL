#include "SignalInteractable.h"
#include "LastSignalGameMode.h"
#include "LastSignalCharacter.h"
#include "SignalInteractionRules.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
ASignalInteractable::ASignalInteractable()
{
    PrimaryActorTick.bCanEverTick = false;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    InteractionName = FText::FromString(TEXT("Unknown signal source"));
}
void ASignalInteractable::Interact_Implementation(AActor* InteractingActor)
{
    const ALastSignalCharacter* Character = Cast<ALastSignalCharacter>(InteractingActor);
    UWorld* World = GetWorld();
    ALastSignalGameMode* Mode = World ? World->GetAuthGameMode<ALastSignalGameMode>() : nullptr;
    if (!Mode) return;

    const bool bHasValidPlayer = Character != nullptr;
    const bool bPlayerAlive = Character && !Character->IsDead();
    if (!LastSignal::CanActivateSignalTerminal(
        bHasBeenActivated, bHasValidPlayer, bPlayerAlive, Mode->AreObjectivesComplete()
    )) return;

    Mode->RegisterSignalCollected();
    bHasBeenActivated = true;
    UE_LOG(LogTemp, Log, TEXT("Relay terminal activated: %s"), *GetName());
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.5f, FColor::Green,
            FString::Printf(TEXT("TERMINAL ONLINE: %s"), *InteractionName.ToString())
        );
    }
}
