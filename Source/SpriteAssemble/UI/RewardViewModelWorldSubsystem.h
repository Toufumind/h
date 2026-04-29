#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ViewModelTypes.h"
#include "RewardViewModelWorldSubsystem.generated.h"

UCLASS()
class SPRITEASSEMBLE_API URewardViewModelWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "UI|ViewModel")
	TArray<FRewardOptionViewSnapshot> GetRewardOptions() const;
};
