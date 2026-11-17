#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LastSignalHUD.generated.h"

UCLASS()
class THELASTSIGNAL_API ALastSignalHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
protected:
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    float BarWidth = 220.0f;
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    float BarHeight = 14.0f;
};
