#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayableRunPlayerController.generated.h"

class UUICommandControllerComponent;

UCLASS()
class SPRITEASSEMBLE_API APlayableRunPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

private:
	void JumpPressed();
	void JumpReleased();
	void FirePressed();
	void SpawnTestEnemy();
	void StartEncounter();
	void ClaimRewardOne();
	void ClaimRewardTwo();
	void ClaimRewardThree();
	void RestartCurrentLevel();
	void ClaimReward(int32 RewardIndex);
	UUICommandControllerComponent* FindCommandController() const;

	UPROPERTY(EditAnywhere, Category = "3C|Control")
	float HorizontalInputDeadZone = 0.1f;

	UPROPERTY(EditAnywhere, Category = "3C|Control")
	FName PlayableNodeId = TEXT("Validation");

	UPROPERTY(EditAnywhere, Category = "3C|Control")
	FName RestartMapName = TEXT("M_RuntimeValidation");

	UPROPERTY(EditAnywhere, Category = "3C|Test")
	float TestEnemySpawnDistance = 360.0f;
};
