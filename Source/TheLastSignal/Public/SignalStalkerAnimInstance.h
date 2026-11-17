#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SignalStalkerAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class THELASTSIGNAL_API USignalStalkerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    float GroundSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    bool bIsMoving = false;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    bool bIsChasing = false;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    bool bIsSearching = false;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    bool bIsAttacking = false;

    UPROPERTY(BlueprintReadOnly, Category = "Stalker|Animation")
    bool bIsDead = false;
};
