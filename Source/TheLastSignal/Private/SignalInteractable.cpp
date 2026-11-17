#include "SignalInteractable.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

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
    bHasBeenActivated = true;
    UE_LOG(LogTemp, Log, TEXT("Signal interaction activated: %s"), *GetName());
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green,
            FString::Printf(TEXT("Signal acquired: %s"), *InteractionName.ToString()));
    }
}
