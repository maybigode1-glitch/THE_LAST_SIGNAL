#include "LastSignalGameMode.h"
#include "LastSignalCharacter.h"
#include "Engine/Engine.h"
ALastSignalGameMode::ALastSignalGameMode() { DefaultPawnClass = ALastSignalCharacter::StaticClass(); }
void ALastSignalGameMode::RegisterSignalCollected()
{
    if (AreObjectivesComplete()) return;
    ++SignalsCollected;
    if (GEngine)
    {
        const bool bComplete = AreObjectivesComplete();
        const FString Message = bComplete ? TEXT("Objective complete: all signals recovered") : FString::Printf(TEXT("Signal recovered: %d/%d"), SignalsCollected, RequiredSignals);
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, bComplete ? FColor::Green : FColor::Cyan, Message);
    }
}
