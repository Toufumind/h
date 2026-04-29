#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Core/GameplaySpecs.h"
#include "EnemyDefinition.generated.h"

class UEnemyActionDefinition;

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UEnemyDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	FName EnemyId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float ContactDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float DetectionRange = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float AttackRange = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UEnemyActionDefinition> DefaultAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	EGameplayTeam Team = EGameplayTeam::Enemy;
};
