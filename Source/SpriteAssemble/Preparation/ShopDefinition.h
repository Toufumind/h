#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Economy/PriceRule.h"
#include "../Reward/RewardTypes.h"
#include "ShopDefinition.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FPreparationShopItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	FRewardOption RewardOption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	FPriceRule PriceRule;
};

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UShopDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	TArray<FPreparationShopItem> Stock;
};
