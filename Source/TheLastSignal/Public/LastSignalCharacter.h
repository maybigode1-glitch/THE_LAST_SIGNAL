#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LastSignalCharacter.generated.h"
class UCameraComponent;

UCLASS()
class THELASTSIGNAL_API ALastSignalCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ALastSignalCharacter();
protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void TryInteract();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FirstPersonCamera;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "50.0"))
    float InteractionDistance = 275.0f;
};
