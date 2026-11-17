#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LastSignalGameMode.generated.h"
UCLASS()
class THELASTSIGNAL_API ALastSignalGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ALastSignalGameMode();
    UFUNCTION(BlueprintCallable, Category = "Objectives") void RegisterSignalCollected();
    UFUNCTION(BlueprintPure, Category = "Objectives") int32 GetSignalsCollected() const { return SignalsCollected; }
    UFUNCTION(BlueprintPure, Category = "Objectives") int32 GetRequiredSignals() const { return RequiredSignals; }
    UFUNCTION(BlueprintPure, Category = "Objectives") bool AreObjectivesComplete() const { return SignalsCollected >= RequiredSignals; }
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Objectives", meta = (ClampMin = "1")) int32 RequiredSignals = 3;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Objectives") int32 SignalsCollected = 0;
};
