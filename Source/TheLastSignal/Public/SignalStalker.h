#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SignalStalker.generated.h"

class AAIController;
class UHealthComponent;

UCLASS(Blueprintable)
class THELASTSIGNAL_API ASignalStalker : public ACharacter
{
    GENERATED_BODY()

public:
    ASignalStalker();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception", meta = (ClampMin = "100.0"))
    float DetectionRadius = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (ClampMin = "50.0"))
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (ClampMin = "0.1"))
    float AttackInterval = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (ClampMin = "0.0"))
    float AttackDamage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol", meta = (ClampMin = "0.0"))
    float PatrolRadius = 700.0f;

protected:
    virtual void BeginPlay() override;

private:
    void UpdateBehaviour();
    void MoveToLocation(const FVector& Destination);
    void PerformAttack();

    FVector HomeLocation = FVector::ZeroVector;
    FVector LastKnownPlayerLocation = FVector::ZeroVector;
    float NextDecisionTime = 0.0f;
    float NextAttackTime = 0.0f;
    bool bHasLastKnownPlayerLocation = false;
    bool bSearchingLastKnownLocation = false;
};
