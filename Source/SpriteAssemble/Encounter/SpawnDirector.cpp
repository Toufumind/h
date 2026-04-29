#include "SpawnDirector.h"
#include "../Enemy/EnemyBase.h"
#include "Engine/World.h"

AActor* USpawnDirector::SpawnEnemy(UWorld* World, const FEnemySpawnEntry& SpawnEntry, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints) const
{
	if (!World || !SpawnEntry.EnemyClass)
	{
		return nullptr;
	}

	AActor* SpawnPoint = PickSpawnPoint(SpawnEntry, SpawnPoints);
	const FTransform SpawnTransform = SpawnPoint ? SpawnPoint->GetActorTransform() : FTransform::Identity;

	AActor* SpawnedEnemy = World->SpawnActor<AActor>(SpawnEntry.EnemyClass, SpawnTransform);
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(SpawnedEnemy))
	{
		EnemyBase->ApplyDefinition(SpawnEntry.EnemyDefinition);
	}

	return SpawnedEnemy;
}

AActor* USpawnDirector::PickSpawnPoint(const FEnemySpawnEntry& SpawnEntry, const TArray<TWeakObjectPtr<AActor>>& SpawnPoints) const
{
	for (const TWeakObjectPtr<AActor>& SpawnPoint : SpawnPoints)
	{
		AActor* Actor = SpawnPoint.Get();
		if (!Actor)
		{
			continue;
		}

		if (SpawnEntry.SpawnPointTag.IsNone() || Actor->ActorHasTag(SpawnEntry.SpawnPointTag))
		{
			return Actor;
		}
	}

	return nullptr;
}
