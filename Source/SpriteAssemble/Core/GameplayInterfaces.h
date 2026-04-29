#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayCommand.h"
#include "GameplaySpecs.h"
#include "GameplayInterfaces.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Damage")
	FGameplayCommandResult ApplyDamageSpec(const FDamageSpec& DamageSpec);
};

UINTERFACE(MinimalAPI, Blueprintable)
class UTargetable : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API ITargetable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Target")
	bool IsTargetable() const;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UTeamAgent : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API ITeamAgent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Team")
	EGameplayTeam GetGameplayTeam() const;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UCommandReceiver : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API ICommandReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Command")
	FGameplayCommandResult ExecuteGameplayCommand(const FGameplayCommand& Command);
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FPresentationState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer StateTags;

	UPROPERTY(BlueprintReadOnly)
	FVector FacingDirection = FVector::ForwardVector;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UPresentationStateProvider : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API IPresentationStateProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Presentation")
	FPresentationState GetPresentationState() const;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UShotHitSpecConsumer : public UInterface
{
	GENERATED_BODY()
};

class SPRITEASSEMBLE_API IShotHitSpecConsumer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Shot")
	void ConsumeShotHitSpec(const FShotHitSpec& HitSpec, TArray<FGameplayCommand>& OutCommands);
};
