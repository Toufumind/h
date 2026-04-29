#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "RunState.h"
#include "RunStateWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;

UCLASS()
class SPRITEASSEMBLE_API URunStateWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Run")
	const FRunState& GetRunState() const { return RunState; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Run")
	void SetPhase(ERunPhase NewPhase);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Run")
	void SetPendingRewardOptions(const TArray<FRewardSpec>& RewardOptions);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Run")
	void ClearPendingRewardOptions();

private:
	FRunState RunState;

	FDelegateHandle AddRedCoreHandle;
	FDelegateHandle SpendRedCoreHandle;
	FDelegateHandle AddGemSlotHandle;
	FDelegateHandle AddPotionMaxHandle;
	FDelegateHandle CompleteNodeHandle;

	FGameplayCommandResult HandleAddRedCore(const FGameplayCommand& Command);
	FGameplayCommandResult HandleSpendRedCore(const FGameplayCommand& Command);
	FGameplayCommandResult HandleAddGemSlot(const FGameplayCommand& Command);
	FGameplayCommandResult HandleAddPotionMax(const FGameplayCommand& Command);
	FGameplayCommandResult HandleCompleteNode(const FGameplayCommand& Command);
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
	void BroadcastCurrencyChanged() const;
	void BroadcastNodeCompleted(const FGameplayCommand& Command) const;
};
