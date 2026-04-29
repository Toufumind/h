#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AttributeTypes.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FAttributeValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdditiveModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MultiplicativeModifier = 1.0f;

	float GetFinalValue() const
	{
		return (BaseValue + AdditiveModifier) * MultiplicativeModifier;
	}
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FAttributeModifierSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdditiveValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MultiplicativeValue = 1.0f;
};
