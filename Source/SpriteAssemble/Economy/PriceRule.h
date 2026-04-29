#pragma once

#include "CoreMinimal.h"
#include "PriceRule.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FPriceRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Economy")
	int32 BasePrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Economy")
	int32 PurchaseCountMultiplier = 0;

	int32 GetPrice(int32 PurchaseCount) const
	{
		return FMath::Max(0, BasePrice + PurchaseCountMultiplier * FMath::Max(0, PurchaseCount));
	}
};
