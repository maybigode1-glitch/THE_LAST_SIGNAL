#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SignalStalker.generated.h"

class UAnimMontage;
class UAudioComponent;
class UStaticMeshComponent;
class USoundBase;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> FootstepSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> AlertSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> SearchSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> AmbientLoopSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> ChaseLoopSound;

    UFUNCTION(BlueprintPure, Category = "Stalker|Animation") bool IsChasing() const { return bIsChasing; }
    UFUNCTION(BlueprintPure, Category = "Stalker|Animation") bool IsSearching() const { return bIsSearching; }
    UFUNCTION(BlueprintPure, Category = "Stalker|Animation") bool IsAttacking() const { return bIsAttacking; }
    UFUNCTION(BlueprintPure, Category = "Stalker|Animation") bool IsDead() const { return bIsDead; }

protected:
    virtual void BeginPlay() override;

private:
    void UpdateBehaviour();
    void PerformAttack();
    void ApplyAttackDamage();
    void SetChasingState(bool bNewChasing);
    void SetSearchingState(bool bNewSearching);
    void UpdateLoopAudio();
    void PlayOneShot(USoundBase* Sound);
    void ClearAttackState();

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    TObjectPtr<UAudioComponent> LoopAudio;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> GreyboxVisual;

    FVector HomeLocation = FVector::ZeroVector;
    FVector LastKnownPlayerLocation = FVector::ZeroVector;
    FVector LastFootstepLocation = FVector::ZeroVector;
    float NextDecisionTime = 0.0f;
    float NextAttackTime = 0.0f;
    float FootstepDistance = 190.0f;
    bool bHasLastKnownPlayerLocation = false;
    bool bSearchingLastKnownLocation = false;
    bool bIsChasing = false;
    bool bIsSearching = false;
    bool bIsAttacking = false;
    bool bIsDead = false;
    FTimerHandle AttackStateTimer;
    FTimerHandle AttackImpactTimer;
};
