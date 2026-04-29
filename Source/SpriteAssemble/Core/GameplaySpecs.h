#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayIds.h"
#include "GameplaySpecs.generated.h"

UENUM(BlueprintType)
enum class EGameplayTeam : uint8
{
	Neutral,
	Player,
	Enemy
};

UENUM(BlueprintType)
enum class ESpellTriggerType : uint8
{
	PreCast,
	OnBuildShot,
	OnBuildDamage,
	OnShotHit,
	OnKill,
	OnDamaged,
	OnTimer
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FAttackSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	EGameplayTeam Team = EGameplayTeam::Neutral;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Cooldown = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Range = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FDamageSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	EGameplayTeam Team = EGameplayTeam::Neutral;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float FinalDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Knockback = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FSpellExecutionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FGemRuntimeId> EquippedGemRuntimeIds;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentGemIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 NextGemIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 ChainDepth = 0;

	UPROPERTY(BlueprintReadWrite)
	ESpellTriggerType TriggerType = ESpellTriggerType::PreCast;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FShotId SourceShotId;

	UPROPERTY(BlueprintReadWrite)
	FGuid EventId;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowSplit = true;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowRecursiveGeneration = false;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxGeneratedShotCount = 128;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxChainDepth = 8;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FShotSpawnSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 ShotCount = 1;

	UPROPERTY(BlueprintReadWrite)
	float SpreadAngle = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FTransform SpawnTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadWrite)
	float Speed = 1000.0f;

	UPROPERTY(BlueprintReadWrite)
	float Radius = 10.0f;

	UPROPERTY(BlueprintReadWrite)
	float Lifetime = 1.5f;

	UPROPERTY(BlueprintReadWrite)
	int32 PierceCount = 0;

	UPROPERTY(BlueprintReadWrite)
	float PostPierceDamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	FName VisualType = TEXT("Default");

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	EGameplayTeam Team = EGameplayTeam::Neutral;

	UPROPERTY(BlueprintReadWrite)
	int32 NextGemIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	FDamageSpec DamageSpec;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FShotHitSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FShotId ShotId;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector HitNormal = FVector::UpVector;

	UPROPERTY(BlueprintReadWrite)
	int32 NextGemIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	float AppliedDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bKilledTarget = false;

	UPROPERTY(BlueprintReadWrite)
	FDamageSpec DamageSpec;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FStatusEffectSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FGameplayTag EffectPolicy;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	int32 StackCount = 1;

	UPROPERTY(BlueprintReadWrite)
	float Magnitude = 0.0f;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FAreaEffectSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EffectTag;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	EGameplayTeam Team = EGameplayTeam::Neutral;

	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float Radius = 100.0f;

	UPROPERTY(BlueprintReadWrite)
	float Delay = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FDamageSpec DamageSpec;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRewardSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag RewardTag;

	UPROPERTY(BlueprintReadWrite)
	FName RewardId = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	int32 Quantity = 1;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};
