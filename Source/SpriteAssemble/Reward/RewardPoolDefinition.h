#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RewardTypes.h"
#include "RewardPoolDefinition.generated.h"

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API URewardPoolDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Reward")
	TArray<FRewardOption> RewardOptions;
};
