#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayableRunHUD.generated.h"

UCLASS()
class SPRITEASSEMBLE_API APlayableRunHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawLineText(const FString& Text, float& Y, const FLinearColor& Color, float Scale = 1.0f) const;
};
