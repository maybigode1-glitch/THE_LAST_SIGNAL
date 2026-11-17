#include "SignalStalker.h"
#include "SignalStalkerAnimInstance.h"
#include "LastSignalCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASignalStalker::ASignalStalker()
{
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCharacterMovement()->MaxWalkSpeed = 230.0f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    LoopAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("StalkerLoopAudio"));
    LoopAudio->SetupAttachment(GetRootComponent());
    LoopAudio->bAutoActivate = false;
}

void ASignalStalker::BeginPlay()
{
    Super::BeginPlay();
    HomeLocation = GetActorLocation();
    LastFootstepLocation = GetActorLocation();
    NextDecisionTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    if (AmbientLoopSound && LoopAudio)
    {
        LoopAudio->SetSound(AmbientLoopSound);
        LoopAudio->Play();
    }
}

void ASignalStalker::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!GetWorld()) return;

    const FVector CurrentLocation = GetActorLocation();
    const float Speed2D = GetVelocity().Size2D();
    if (Speed2D > 35.0f && FootstepSound &&
        FVector::DistSquared(CurrentLocation, LastFootstepLocation) >= FMath::Square(FootstepDistance))
    {
        PlayOneShot(FootstepSound);
        LastFootstepLocation = CurrentLocation;
    }
    else if (Speed2D <= 35.0f)
    {
        LastFootstepLocation = CurrentLocation;
    }

    if (!GetController()) return;
    if (GetWorld()->GetTimeSeconds() >= NextDecisionTime)
    {
        UpdateBehaviour();
        NextDecisionTime = GetWorld()->GetTimeSeconds() + 0.25f;
    }
}

void ASignalStalker::SetChasingState(bool bNewChasing)
{
    if (bIsChasing == bNewChasing) return;
    bIsChasing = bNewChasing;
    UpdateLoopAudio();
}

void ASignalStalker::SetSearchingState(bool bNewSearching)
{
    if (bIsSearching == bNewSearching) return;
    bIsSearching = bNewSearching;
    if (bIsSearching) PlayOneShot(SearchSound);
}

void ASignalStalker::UpdateLoopAudio()
{
    if (!LoopAudio) return;
    USoundBase* DesiredSound = bIsChasing && ChaseLoopSound ? ChaseLoopSound : AmbientLoopSound;
    if (DesiredSound && LoopAudio->Sound != DesiredSound)
    {
        LoopAudio->SetSound(DesiredSound);
        LoopAudio->Play();
    }
    else if (!DesiredSound)
    {
        LoopAudio->Stop();
    }
}

void ASignalStalker::PlayOneShot(USoundBase* Sound)
{
    if (Sound && GetWorld())
        UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
}

void ASignalStalker::UpdateBehaviour()
{
    AAIController* AI = Cast<AAIController>(GetController());
    ALastSignalCharacter* Player = Cast<ALastSignalCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!AI || !Player || Player->IsDead())
    {
        SetChasingState(false);
        return;
    }

    const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    const bool bCanSeePlayer = Distance <= DetectionRadius && AI->LineOfSightTo(Player);

    if (bCanSeePlayer)
    {
        if (!bIsChasing) PlayOneShot(AlertSound);
        SetChasingState(true);
        SetSearchingState(false);
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

    SetChasingState(false);
    if (bHasLastKnownPlayerLocation)
    {
        if (!bSearchingLastKnownLocation)
        {
            SetSearchingState(true);
            AI->MoveToLocation(LastKnownPlayerLocation, 75.0f, true);
            bSearchingLastKnownLocation = true;
        }
        else if (AI->GetMoveStatus() == EPathFollowingStatus::Idle)
        {
            bHasLastKnownPlayerLocation = false;
            SetSearchingState(false);
            AI->MoveToLocation(HomeLocation, 90.0f, true);
        }
        return;
    }

    SetSearchingState(false);
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
    if (!GetWorld() || GetWorld()->GetTimeSeconds() < NextAttackTime || bIsAttacking) return;
    ALastSignalCharacter* Player = Cast<ALastSignalCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player || Player->IsDead()) return;

    bIsAttacking = true;
    if (USkeletalMeshComponent* CharacterMesh = GetMesh())
    {
        if (AttackMontage)
        {
            if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
                AnimInstance->Montage_Play(AttackMontage);
        }
    }
    PlayOneShot(AttackSound);
    Player->TakeDamage(AttackDamage, FDamageEvent(), GetController(), this);
    NextAttackTime = GetWorld()->GetTimeSeconds() + AttackInterval;

    const float AttackStateDuration = AttackMontage ? FMath::Max(0.25f, AttackMontage->GetPlayLength()) : 0.45f;
    GetWorldTimerManager().SetTimer(AttackStateTimer, this, &ASignalStalker::ClearAttackState, AttackStateDuration, false);
}

void ASignalStalker::ClearAttackState()
{
    bIsAttacking = false;
}
