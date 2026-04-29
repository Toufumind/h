#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveConfig.h"
#include "WaveDirector.generated.h"

class USpawnDirector;

UCLASS()
class SPRITEASSEMBLE_API UWaveDirector : public UObject
{
	GENERATED_BODY()

public:
	void StartWave(UWaveConfig* InWaveConfig);
	void TickWave(float DeltaTime, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints, USpawnDirector* SpawnDirector, TArray<AActor*>& OutSpawnedEnemies);

	bool HasSpawnedEverything() const;
	bool IsRunning() const { return WaveConfig != nullptr; }

private:
	UPROPERTY()
	TObjectPtr<UWaveConfig> WaveConfig = nullptr;

	int32 CurrentEntryIndex = 0;
	int32 SpawnedFromCurrentEntry = 0;
	float SpawnTimer = 0.0f;
};
