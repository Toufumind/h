#include "WaveDirector.h"
#include "SpawnDirector.h"

void UWaveDirector::StartWave(UWaveConfig* InWaveConfig)
{
	WaveConfig = InWaveConfig;
	CurrentEntryIndex = 0;
	SpawnedFromCurrentEntry = 0;
	SpawnTimer = 0.0f;
}

void UWaveDirector::TickWave(float DeltaTime, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints, USpawnDirector* SpawnDirector, TArray<AActor*>& OutSpawnedEnemies)
{
	if (!WaveConfig || !SpawnDirector || HasSpawnedEverything())
	{
		return;
	}

	SpawnTimer -= DeltaTime;
	if (SpawnTimer > 0.0f)
	{
		return;
	}

	while (WaveConfig->SpawnEntries.IsValidIndex(CurrentEntryIndex))
	{
		const FEnemySpawnEntry& Entry = WaveConfig->SpawnEntries[CurrentEntryIndex];
		if (SpawnedFromCurrentEntry < FMath::Max(0, Entry.Count))
		{
			if (AActor* SpawnedEnemy = SpawnDirector->SpawnEnemy(World, Entry, SpawnPoints))
			{
				OutSpawnedEnemies.Add(SpawnedEnemy);
			}

			++SpawnedFromCurrentEntry;
			SpawnTimer = FMath::Max(0.0f, Entry.SpawnInterval);
			return;
		}

		++CurrentEntryIndex;
		SpawnedFromCurrentEntry = 0;
	}
}

bool UWaveDirector::HasSpawnedEverything() const
{
	if (!WaveConfig)
	{
		return true;
	}

	return CurrentEntryIndex >= WaveConfig->SpawnEntries.Num();
}
