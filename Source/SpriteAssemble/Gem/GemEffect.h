#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Core/GameplayCommand.h"
#include "../Core/GameplaySpecs.h"
#include "GemEffect.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class SPRITEASSEMBLE_API UGemEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void ModifyAttackSpec(FAttackSpec& AttackSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const {}
	virtual void ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const {}
	virtual void ModifyDamageSpec(FDamageSpec& DamageSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const {}
	virtual void HandleShotHitSpec(const FShotHitSpec& HitSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const {}
};
