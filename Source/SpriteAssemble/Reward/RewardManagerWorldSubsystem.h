#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "RewardTypes.h"
#include "RewardManagerWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;
class URewardPoolDefinition;

UCLASS()
class SPRITEASSEMBLE_API URewardManagerWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Reward")
	const TArray<FRewardOption>& GetCurrentRewardOptions() const { return CurrentRewardOptions; }

	// Editor/smoke bootstrap only. Production content must wire RewardPool through RunConfig.
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Reward")
	void SetRewardPool(URewardPoolDefinition* InRewardPool) { RewardPoolOverride = InRewardPool; }

private:
	UPROPERTY()
	TObjectPtr<URewardPoolDefinition> RewardPoolOverride = nullptr;

	UPROPERTY()
	TArray<FRewardOption> CurrentRewardOptions;

	FDelegateHandle BuildRewardOptionsHandle;
	FDelegateHandle ClaimRewardHandle;

	FGameplayCommandResult HandleBuildRewardOptions(const FGameplayCommand& Command);
	FGameplayCommandResult HandleClaimReward(const FGameplayCommand& Command);
	URewardPoolDefinition* ResolveRewardPool() const;
	FGameplayCommand BuildApplyCommand(const FRewardOption& RewardOption, AActor* ClaimingActor) const;
	void BroadcastRewardOptionsBuilt() const;
	void BroadcastRewardClaimed(const FRewardOption& RewardOption) const;
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
};
