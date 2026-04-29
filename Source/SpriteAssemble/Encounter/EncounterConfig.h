#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EncounterConfig.generated.h"

class UWaveConfig;

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UEncounterConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	FName EncounterId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	TArray<TObjectPtr<UWaveConfig>> Waves;
};
