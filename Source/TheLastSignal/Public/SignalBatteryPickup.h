#pragma once
#include "CoreMinimal.h"
#include "SignalInteractable.h"
#include "SignalBatteryPickup.generated.h"
UCLASS(Blueprintable)
class THELASTSIGNAL_API ASignalBatteryPickup : public ASignalInteractable
{
    GENERATED_BODY()
public:
    ASignalBatteryPickup();
    virtual void Interact_Implementation(AActor* InteractingActor) override;
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "1"))
    int32 PacksGranted = 1;
};
