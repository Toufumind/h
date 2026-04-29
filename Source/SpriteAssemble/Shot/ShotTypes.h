#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "ShotTypes.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FShotInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FShotId ShotId;

	UPROPERTY(BlueprintReadOnly)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector PreviousPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadOnly)
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float Radius = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float Lifetime = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	int32 PierceCount = 0;

	UPROPERTY(BlueprintReadOnly)
	float PostPierceDamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	FName VisualType = TEXT("Default");

	UPROPERTY(BlueprintReadOnly)
	int32 InstanceIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	bool bAlive = false;

	UPROPERTY(BlueprintReadOnly)
	int32 NextGemIndex = 0;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EGameplayTeam Team = EGameplayTeam::Neutral;

	UPROPERTY(BlueprintReadOnly)
	FDamageSpec DamageSpec;
};
