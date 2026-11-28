#include "LastSignalGameMode.h"
#include "LastSignalCharacter.h"
#include "LastSignalHUD.h"
#include "Engine/Engine.h"
ALastSignalGameMode::ALastSignalGameMode()
{
    DefaultPawnClass = ALastSignalCharacter::StaticClass();
    HUDClass = ALastSignalHUD::StaticClass();
}
void ALastSignalGameMode::RegisterSignalCollected()
{
    int32 UpdatedSignalsCollected = SignalsCollected;
    const bool bAccepted = LastSignal::TryRegisterSignalCollected(
        SignalsCollected, RequiredSignals, UpdatedSignalsCollected
    );
    SignalsCollected = UpdatedSignalsCollected;
    if (!bAccepted) return;

    if (GEngine)
    {
        const bool bComplete = AreObjectivesComplete();
        const FString Message = bComplete ? TEXT("Objective complete: all signals recovered") : FString::Printf(TEXT("Signal recovered: %d/%d"), SignalsCollected, GetRequiredSignals());
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, bComplete ? FColor::Green : FColor::Cyan, Message);
    }
}
