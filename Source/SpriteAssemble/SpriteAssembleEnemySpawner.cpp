// SpriteAssembleEnemySpawner.cpp
#include "SpriteAssembleEnemySpawner.h"
#include "SpriteAssembleEnemyBase.h"

ASpriteAssembleEnemySpawner::ASpriteAssembleEnemySpawner() { PrimaryActorTick.bCanEverTick = false; }

void ASpriteAssembleEnemySpawner::SpawnEnemies()
{
	if (EnemyClassesToSpawn.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyClassesToSpawn.Num() - 1);
		if (EnemyClassesToSpawn[RandomIndex])
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor<ASpriteAssembleEnemyBase>(EnemyClassesToSpawn[RandomIndex], GetActorLocation(), GetActorRotation(), SpawnParams);
		}
	}
}

