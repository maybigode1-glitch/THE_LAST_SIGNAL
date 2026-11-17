#include "SignalStalker.h"
#include "LastSignalCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

ASignalStalker::ASignalStalker()
{
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCharacterMovement()->MaxWalkSpeed = 230.0f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
}

void ASignalStalker::BeginPlay()
{
    Super::BeginPlay();
    HomeLocation = GetActorLocation();
    NextDecisionTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

void ASignalStalker::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!GetWorld() || !GetController()) return;
    if (GetWorld()->GetTimeSeconds() >= NextDecisionTime)
    {
        UpdateBehaviour();
        NextDecisionTime = GetWorld()->GetTimeSeconds() + 0.25f;
    }
}

void ASignalStalker::UpdateBehaviour()
{
    AAIController* AI = Cast<AAIController>(GetController());
    ALastSignalCharacter* Player = Cast<ALastSignalCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!AI || !Player || Player->IsDead()) return;

    const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    const bool bCanSeePlayer = Distance <= DetectionRadius && AI->LineOfSightTo(Player);

    if (bCanSeePlayer)
    {
        LastKnownPlayerLocation = Player->GetActorLocation();
        bHasLastKnownPlayerLocation = true;
        bSearchingLastKnownLocation = false;

        if (Distance <= AttackRange)
        {
            AI->StopMovement();
            PerformAttack();
        }
        else
        {
            AI->MoveToActor(Player, AttackRange * 0.65f, true, true, true, nullptr, true);
        }
        return;
    }

    if (bHasLastKnownPlayerLocation)
    {
        if (!bSearchingLastKnownLocation)
        {
            AI->MoveToLocation(LastKnownPlayerLocation, 75.0f, true);
            bSearchingLastKnownLocation = true;
        }
        else if (AI->GetMoveStatus() == EPathFollowingStatus::Idle)
        {
            bHasLastKnownPlayerLocation = false;
            AI->MoveToLocation(HomeLocation, 90.0f, true);
        }
        return;
    }

    if (FVector::DistSquared(GetActorLocation(), HomeLocation) > FMath::Square(PatrolRadius))
    {
        AI->MoveToLocation(HomeLocation, 100.0f, true);
        return;
    }

    if (AI->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        FNavLocation PatrolPoint;
        if (UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
        {
            if (Navigation->GetRandomReachablePointInRadius(HomeLocation, PatrolRadius, PatrolPoint))
                AI->MoveToLocation(PatrolPoint.Location, 80.0f, true);
        }
    }
}

void ASignalStalker::PerformAttack()
{
    if (!GetWorld() || GetWorld()->GetTimeSeconds() < NextAttackTime) return;
    ALastSignalCharacter* Player = Cast<ALastSignalCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player || Player->IsDead()) return;
    Player->TakeDamage(AttackDamage, FDamageEvent(), GetController(), this);
    NextAttackTime = GetWorld()->GetTimeSeconds() + AttackInterval;
}
