#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GemTypes.h"
#include "GemDefinition.generated.h"

class UGemEffect;

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UGemDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	FName GemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	EGemSlotEffectType EffectType = EGemSlotEffectType::Function;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	EGemRarity Rarity = EGemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 MaxLevel = 3;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Gem")
	TObjectPtr<UGemEffect> Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	FGameplayTagContainer Tags;
};
