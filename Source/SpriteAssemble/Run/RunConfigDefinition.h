#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RunConfigDefinition.generated.h"

namespace SpriteAssembleRunConfigKeys
{
	static const FName RewardPool(TEXT("RewardPool"));
}

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRunConfigObjectEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Run")
	FName ConfigKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Run")
	TObjectPtr<UObject> ConfigObject = nullptr;
};

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API URunConfigDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Run")
	TArray<FRunConfigObjectEntry> ConfigObjects;
};
