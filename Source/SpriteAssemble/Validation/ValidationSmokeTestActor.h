#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ValidationSmokeTestActor.generated.h"

class AEnemyBase;
class ASpriteAssembleCharacter;
class ASpriteAssembleProjectile;
class AShotInstancedMeshView;
class URewardPoolDefinition;
class UUICommandControllerComponent;

USTRUCT(BlueprintType)
struct FValidationSmokeSnapshot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 PlayerCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 EnemyBaseCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bHasShotInstancedMeshView = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerHasPlayerMotorComponent = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerHasDamageReactionComponent = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerHasVisibleMarkerPlane = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bEnemyHasVisibleMarkerPlane = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float PlayerHorizontalSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerIsJumpBuffered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerIsUsingCoyoteTime = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerIsInvincible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bPlayerIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bFoundLegacyProjectileActor = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	FName CommandOwnerName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	TArray<FName> PlayerActorNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	FString RunPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 CompletedNodeCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 RedCore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 GemSlotCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 RewardOptionsCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	TArray<FName> RewardOptionNames;
};

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API AValidationSmokeTestActor : public AActor
{
	GENERATED_BODY()

public:
	AValidationSmokeTestActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
	bool ConfigureRewardPool();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
	void TriggerSelectNode();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
	void TriggerClaimReward();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
	void RunSmoke();

	UFUNCTION(BlueprintCallable, Category = "Validation")
	FValidationSmokeSnapshot CaptureSnapshot() const;

	UFUNCTION(Exec)
	void ValidationRunSmoke();

	UFUNCTION(Exec)
	void ValidationSelectNode();

	UFUNCTION(Exec)
	void ValidationClaimReward();

	UFUNCTION(Exec)
	void ValidationSnapshot();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation")
	TObjectPtr<URewardPoolDefinition> RewardPool = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation")
	TObjectPtr<AActor> CommandOwner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation")
	FName NodeId = TEXT("Validation");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation")
	int32 RewardOptionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation")
	bool bRunSmokeOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation", meta = (ClampMin = "0.0"))
	float ClaimRewardDelaySeconds = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Validation", meta = (ClampMin = "0.0"))
	float SnapshotDelaySeconds = 0.2f;

private:
	void ClaimRewardAndCaptureSnapshot();
	UUICommandControllerComponent* FindCommandController() const;
	AActor* FindCommandOwner() const;
	void LogSnapshot(const FValidationSmokeSnapshot& Snapshot) const;
};
