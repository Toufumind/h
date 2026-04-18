// SpriteAssembleEnemySpawner.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteAssembleEnemySpawner.generated.h"

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleEnemySpawner : public AActor
{
	GENERATED_BODY()
public:
	ASpriteAssembleEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<class ASpriteAssembleEnemyBase>> EnemyClassesToSpawn;

	// 被祭坛或关卡管理器调用
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemies();
};

