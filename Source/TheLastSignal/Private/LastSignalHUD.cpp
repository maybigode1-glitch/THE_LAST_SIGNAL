#include "LastSignalHUD.h"
#include "LastSignalCharacter.h"
#include "LastSignalGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasItem.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"

void ALastSignalHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;

    const ALastSignalCharacter* Character = Cast<ALastSignalCharacter>(GetOwningPawn());
    const ALastSignalGameMode* Mode = GetWorld() ? GetWorld()->GetAuthGameMode<ALastSignalGameMode>() : nullptr;
    const float Scale = FMath::Clamp(Canvas->ClipY / 1080.0f, 0.75f, 1.35f);
    const float Left = 36.0f * Scale;
    const float Top = 34.0f * Scale;
    const float Width = BarWidth * Scale;
    const float Height = BarHeight * Scale;
    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    if (!Font) return;

    auto DrawLabel = [this, Font, Scale](const FString& Text, float X, float Y, const FLinearColor& Color)
    {
        const FLinearColor PreviousColor = Canvas->DrawColor;
        Canvas->DrawColor = Color;
        Canvas->DrawText(Font, Text, X, Y, Scale, Scale);
        Canvas->DrawColor = PreviousColor;
    };

    DrawLabel(TEXT("THE LAST SIGNAL"), Left, Top, FLinearColor::White);

    if (Character)
    {
        const float Battery = FMath::Clamp(Character->GetFlashlightBatteryPercent(), 0.0f, 100.0f);
        const FString LightState = Character->IsFlashlightOn() ? TEXT("ON") : TEXT("OFF");
        const float BarY = Top + 34.0f * Scale;
        FCanvasTileItem Back(FVector2D(Left, BarY), FVector2D(Width, Height), FLinearColor(0.035f, 0.045f, 0.055f, 0.92f));
        Back.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(Back);
        const FLinearColor FillColor = Battery <= 20.0f ? FLinearColor(0.85f, 0.12f, 0.08f, 1.0f) : FLinearColor(0.25f, 0.78f, 0.68f, 1.0f);
        FCanvasTileItem Fill(FVector2D(Left, BarY), FVector2D(Width * Battery / 100.0f, Height), FillColor);
        Fill.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(Fill);
        DrawLabel(FString::Printf(TEXT("FLASHLIGHT  %3.0f%%  [%s]"), Battery, *LightState), Left, BarY + Height + 5.0f * Scale, FLinearColor::White);
        DrawLabel(FString::Printf(TEXT("SPARE BATTERIES  x%d  (R)"), Character->GetBatteryPackCount()), Left, BarY + Height + 29.0f * Scale, FLinearColor::White);
    }

    if (Mode)
    {
        const int32 Current = Mode->GetSignalsCollected();
        const int32 Required = Mode->GetRequiredSignals();
        const FString Objective = Mode->AreObjectivesComplete()
            ? TEXT("SIGNAL NETWORK RESTORED")
            : FString::Printf(TEXT("RECOVER SIGNALS  %d / %d"), Current, Required);
        const float TextWidth = 270.0f * Scale;
        const float X = FMath::Max(Left, Canvas->ClipX - TextWidth - Left);
        DrawLabel(Objective, X, Top, Mode->AreObjectivesComplete() ? FLinearColor(0.25f, 1.0f, 0.55f, 1.0f) : FLinearColor::White);
        const float ObjBarY = Top + 25.0f * Scale;
        FCanvasTileItem ObjBack(FVector2D(X, ObjBarY), FVector2D(TextWidth, Height), FLinearColor(0.035f, 0.045f, 0.055f, 0.92f));
        ObjBack.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(ObjBack);
        const float Progress = Required > 0 ? FMath::Clamp(static_cast<float>(Current) / Required, 0.0f, 1.0f) : 0.0f;
        FCanvasTileItem ObjFill(FVector2D(X, ObjBarY), FVector2D(TextWidth * Progress, Height), FLinearColor(0.95f, 0.62f, 0.18f, 1.0f));
        ObjFill.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(ObjFill);
    }

    const float CenterX = Canvas->ClipX * 0.5f;
    const float CenterY = Canvas->ClipY * 0.5f;
    FCanvasTileItem Crosshair(FVector2D(CenterX - 1.0f, CenterY - 1.0f), FVector2D(2.0f, 2.0f), FLinearColor(0.9f, 0.95f, 0.9f, 0.75f));
    Crosshair.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Crosshair);
}
