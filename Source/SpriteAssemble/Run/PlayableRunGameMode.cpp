#include "PlayableRunGameMode.h"

#include "../SpriteAssembleCharacter.h"
#include "../UI/PlayableRunHUD.h"
#include "../UI/PlayableRunPlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

APlayableRunGameMode::APlayableRunGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = APlayableRunPlayerController::StaticClass();
	HUDClass = APlayableRunHUD::StaticClass();
	bStartPlayersAsSpectators = false;
}

void APlayableRunGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		PossessPlacedPlayer(It->Get());
	}
}

void APlayableRunGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PossessPlacedPlayer(NewPlayer);
}

void APlayableRunGameMode::PossessPlacedPlayer(APlayerController* PlayerController) const
{
	if (!PlayerController || !GetWorld())
	{
		return;
	}

	if (Cast<ASpriteAssembleCharacter>(PlayerController->GetPawn()))
	{
		return;
	}

	for (TActorIterator<ASpriteAssembleCharacter> It(GetWorld()); It; ++It)
	{
		if (APawn* CurrentPawn = PlayerController->GetPawn())
		{
			PlayerController->UnPossess();
			CurrentPawn->Destroy();
		}

		PlayerController->Possess(*It);
		return;
	}
}
