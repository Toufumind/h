#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveConfig.generated.h"

class UEnemyDefinition;

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FEnemySpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	TObjectPtr<UEnemyDefinition> EnemyDefinition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	float SpawnInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	FName SpawnPointTag = NAME_None;
};

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UWaveConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	TArray<FEnemySpawnEntry> SpawnEntries;
};
