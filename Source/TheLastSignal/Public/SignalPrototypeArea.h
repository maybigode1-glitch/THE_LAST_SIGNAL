#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalPrototypeArea.generated.h"

class UStaticMesh;
class USceneComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class THELASTSIGNAL_API ASignalPrototypeArea : public AActor
{
    GENERATED_BODY()
public:
    ASignalPrototypeArea();
protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, Category = "Area")
    TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    bool bSpawnStalker = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    bool bSpawnPickups = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    bool bSpawnSignalObjectives = true;
private:
    UPROPERTY()
    TObjectPtr<UStaticMesh> CubeMesh;
    void AddBlock(const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
    void BuildGeometry();
    void SpawnObjectivesAndPickups();
    void SpawnStalker();
    AActor* SpawnMarkerActor(TSubclassOf<AActor> ActorClass, const FVector& Location, const FVector& Scale, const FText& Label);
};