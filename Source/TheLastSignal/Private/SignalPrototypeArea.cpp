#include "SignalPrototypeArea.h"
#include "SignalInteractable.h"
#include "SignalBatteryPickup.h"
#include "SignalStalker.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ASignalPrototypeArea::ASignalPrototypeArea()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeAsset.Succeeded()) CubeMesh = CubeAsset.Object;
}

void ASignalPrototypeArea::BeginPlay()
{
    Super::BeginPlay();
    BuildGeometry();
    SpawnObjectivesAndPickups();
    if (bSpawnStalker) SpawnStalker();
}

void ASignalPrototypeArea::AddBlock(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
    if (!CubeMesh) return;
    UStaticMeshComponent* Block = NewObject<UStaticMeshComponent>(this);
    if (!Block) return;
    Block->SetStaticMesh(CubeMesh);
    Block->SetMobility(EComponentMobility::Static);
    Block->SetCollisionProfileName(TEXT("BlockAll"));
    Block->SetRelativeLocation(Location);
    Block->SetRelativeRotation(Rotation);
    Block->SetRelativeScale3D(Scale);
    Block->SetupAttachment(SceneRoot);
    AddInstanceComponent(Block);
    Block->RegisterComponent();
}

void ASignalPrototypeArea::BuildGeometry()
{
    // 30 m x 22 m industrial footprint; the floor top is at local Z=0.
    AddBlock(FVector(0, 0, -10), FVector(30, 22, 0.2f));
    // Outer perimeter, 4.5 m high.
    AddBlock(FVector(0, -1100, 225), FVector(30, 0.5f, 4.5f));
    AddBlock(FVector(0, 1100, 225), FVector(30, 0.5f, 4.5f));
    AddBlock(FVector(-1500, 0, 225), FVector(0.5f, 22, 4.5f));
    AddBlock(FVector(1500, 0, 225), FVector(0.5f, 22, 4.5f));
    // Broken interior partitions form a main route with side rooms and cover.
    AddBlock(FVector(-650, -560, 150), FVector(0.18f, 5.8f, 3.0f));
    AddBlock(FVector(-650, 360, 150), FVector(0.18f, 3.8f, 3.0f));
    AddBlock(FVector(100, 600, 150), FVector(5.5f, 0.18f, 3.0f));
    AddBlock(FVector(100, -650, 150), FVector(4.0f, 0.18f, 3.0f));
    AddBlock(FVector(650, 0, 150), FVector(0.18f, 5.5f, 3.0f));
    AddBlock(FVector(1050, 650, 110), FVector(2.2f, 0.25f, 2.2f));
    AddBlock(FVector(1050, -650, 110), FVector(2.2f, 0.25f, 2.2f));
    // Industrial cover crates; deliberately low enough to vault around.
    AddBlock(FVector(-250, 250, 65), FVector(1.5f, 1.2f, 1.3f));
    AddBlock(FVector(400, -250, 65), FVector(1.2f, 1.5f, 1.3f));
    AddBlock(FVector(900, 250, 65), FVector(1.6f, 1.0f, 1.3f));
    AddBlock(FVector(-1050, -250, 65), FVector(1.0f, 1.4f, 1.3f));
}

AActor* ASignalPrototypeArea::SpawnMarkerActor(TSubclassOf<AActor> ActorClass, const FVector& Location, const FVector& Scale, const FText& Label)
{
    if (!GetWorld() || !ActorClass) return nullptr;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass, GetActorTransform().TransformPosition(Location), GetActorRotation(), Params);
    if (!Spawned) return nullptr;
    if (ASignalInteractable* Interactable = Cast<ASignalInteractable>(Spawned))
    {
        Interactable->InteractionName = Label;
        if (UStaticMeshComponent* Mesh = Interactable->FindComponentByClass<UStaticMeshComponent>())
        {
            if (CubeMesh) Mesh->SetStaticMesh(CubeMesh);
            Mesh->SetRelativeScale3D(Scale);
            Mesh->SetRelativeLocation(FVector(0, 0, 55));
            Mesh->SetCollisionProfileName(TEXT("BlockAll"));
        }
    }
    return Spawned;
}

void ASignalPrototypeArea::SpawnObjectivesAndPickups()
{
    if (bSpawnSignalObjectives)
    {
        SpawnMarkerActor(ASignalInteractable::StaticClass(), FVector(-250, -650, 0), FVector(0.65f, 0.65f, 1.1f), FText::FromString(TEXT("Relay Terminal A")));
        SpawnMarkerActor(ASignalInteractable::StaticClass(), FVector(350, 650, 0), FVector(0.65f, 0.65f, 1.1f), FText::FromString(TEXT("Relay Terminal B")));
        SpawnMarkerActor(ASignalInteractable::StaticClass(), FVector(1200, 0, 0), FVector(0.65f, 0.65f, 1.1f), FText::FromString(TEXT("Primary Signal Uplink")));
    }
    if (bSpawnPickups)
    {
        SpawnMarkerActor(ASignalBatteryPickup::StaticClass(), FVector(-950, 600, 0), FVector(0.35f, 0.35f, 0.55f), FText::FromString(TEXT("Battery Pack")));
        SpawnMarkerActor(ASignalBatteryPickup::StaticClass(), FVector(500, -700, 0), FVector(0.35f, 0.35f, 0.55f), FText::FromString(TEXT("Battery Pack")));
    }
}

void ASignalPrototypeArea::SpawnStalker()
{
    if (!GetWorld()) return;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ASignalStalker* Stalker = GetWorld()->SpawnActor<ASignalStalker>(ASignalStalker::StaticClass(), GetActorTransform().TransformPosition(FVector(950, 0, 100)), GetActorRotation(), Params);
    if (!Stalker || !CubeMesh) return;
    // Temporary greybox silhouette until a skeletal mesh and Animation Blueprint are assigned.
    UStaticMeshComponent* Placeholder = NewObject<UStaticMeshComponent>(Stalker, TEXT("GreyboxStalkerVisual"));
    Placeholder->SetStaticMesh(CubeMesh);
    Placeholder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Placeholder->SetRelativeLocation(FVector(0, 0, 35));
    Placeholder->SetRelativeScale3D(FVector(0.42f, 0.42f, 1.25f));
    Placeholder->SetupAttachment(Stalker->GetRootComponent());
    Stalker->AddInstanceComponent(Placeholder);
    Placeholder->RegisterComponent();
}