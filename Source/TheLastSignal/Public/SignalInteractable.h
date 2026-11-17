#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalInteractable.generated.h"
class UStaticMeshComponent;

UCLASS(Blueprintable)
class THELASTSIGNAL_API ASignalInteractable : public AActor
{
    GENERATED_BODY()
public:
    ASignalInteractable();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FText InteractionName;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bHasBeenActivated = false;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* InteractingActor);
    virtual void Interact_Implementation(AActor* InteractingActor);
};
