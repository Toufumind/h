#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveConfig.h"
#include "SpawnDirector.generated.h"

UCLASS()
class SPRITEASSEMBLE_API USpawnDirector : public UObject
{
	GENERATED_BODY()

public:
	AActor* SpawnEnemy(UWorld* World, const FEnemySpawnEntry& SpawnEntry, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints) const;

private:
	AActor* PickSpawnPoint(const FEnemySpawnEntry& SpawnEntry, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints) const;
};
