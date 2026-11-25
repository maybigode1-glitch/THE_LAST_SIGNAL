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
    if (!Character->AddBatteryPack(PacksGranted)) return;
    bHasBeenActivated = true;
    Destroy();
}
