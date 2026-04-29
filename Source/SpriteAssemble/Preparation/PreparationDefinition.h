#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PreparationDefinition.generated.h"

class UShopDefinition;

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UPreparationDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	FName PreparationId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Preparation")
	TObjectPtr<UShopDefinition> Shop = nullptr;
};
