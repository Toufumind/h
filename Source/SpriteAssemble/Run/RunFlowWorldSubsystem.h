#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "../Core/GameplayEvent.h"
#include "RunFlowWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;
class UGameplayEventBus;

UCLASS()
class SPRITEASSEMBLE_API URunFlowWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

private:
	FDelegateHandle NodeSelectedHandle;
	FDelegateHandle EncounterCompletedHandle;
	FDelegateHandle RewardClaimedHandle;
	FDelegateHandle PreparationExitedHandle;

	void HandleNodeSelected(const FGameplayEvent& Event);
	void HandleEncounterCompleted(const FGameplayEvent& Event);
	void HandleRewardClaimed(const FGameplayEvent& Event);
	void HandlePreparationExited(const FGameplayEvent& Event);

	void DispatchCommand(const FGameplayCommand& Command) const;
	UGameplayEventBus* GetEventBus() const;
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
};
