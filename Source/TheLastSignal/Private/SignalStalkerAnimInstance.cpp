#include "SignalStalkerAnimInstance.h"
#include "SignalStalker.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

void USignalStalkerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    const ASignalStalker* Stalker = Cast<ASignalStalker>(TryGetPawnOwner());
    if (!Stalker)
    {
        GroundSpeed = 0.0f;
        bIsMoving = false;
        bIsChasing = false;
        bIsSearching = false;
        bIsAttacking = false;
        bIsDead = false;
        return;
    }

    const FVector Velocity = Stalker->GetVelocity();
    GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
    bIsMoving = GroundSpeed > 8.0f;
    bIsChasing = Stalker->IsChasing();
    bIsSearching = Stalker->IsSearching();
    bIsAttacking = Stalker->IsAttacking();
    bIsDead = Stalker->IsDead();
}
