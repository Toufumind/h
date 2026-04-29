#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyActionDefinition.generated.h"

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UEnemyActionDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	FName ActionId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float TelegraphTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float StartupTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float ActiveTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float RecoveryTime = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float Range = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action")
	bool bUseShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action", meta = (EditCondition = "bUseShot"))
	float ShotSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action", meta = (EditCondition = "bUseShot"))
	float ShotRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action", meta = (EditCondition = "bUseShot"))
	float ShotLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Action", meta = (EditCondition = "bUseShot"))
	FName ShotVisualType = TEXT("EnemyDefault");
};
