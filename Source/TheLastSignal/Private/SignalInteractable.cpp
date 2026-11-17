#include "SignalInteractable.h"
#include "LastSignalGameMode.h"
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
    if (bHasBeenActivated) return;
    bHasBeenActivated = true;
    if (UWorld* World = GetWorld())
        if (ALastSignalGameMode* Mode = World->GetAuthGameMode<ALastSignalGameMode>()) Mode->RegisterSignalCollected();
    UE_LOG(LogTemp, Log, TEXT("Signal interaction activated: %s"), *GetName());
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, FString::Printf(TEXT("Signal acquired: %s"), *InteractionName.ToString()));
}
