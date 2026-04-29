#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayableRunGameMode.generated.h"

UCLASS()
class SPRITEASSEMBLE_API APlayableRunGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlayableRunGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void PossessPlacedPlayer(APlayerController* PlayerController) const;
};
