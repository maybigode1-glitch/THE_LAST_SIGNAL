#include "LastSignalGameMode.h"
#include "LastSignalCharacter.h"
ALastSignalGameMode::ALastSignalGameMode()
{
    DefaultPawnClass = ALastSignalCharacter::StaticClass();
}
