#include "SignalBatteryPickup.h"
#include "LastSignalCharacter.h"
ASignalBatteryPickup::ASignalBatteryPickup()
{
    InteractionName = FText::FromString(TEXT("Battery pack"));
}
void ASignalBatteryPickup::Interact_Implementation(AActor* InteractingActor)
{
    ALastSignalCharacter* Character = Cast<ALastSignalCharacter>(InteractingActor);
    if (!Character || bHasBeenActivated) return;
    Character->AddBatteryPack(PacksGranted);
    bHasBeenActivated = true;
    Destroy();
}
