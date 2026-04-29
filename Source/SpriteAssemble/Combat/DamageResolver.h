#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "../Core/GameplayCommand.h"

class SPRITEASSEMBLE_API FDamageResolver
{
public:
	static FGameplayCommandResult ApplyDamage(AActor* TargetActor, const FDamageSpec& DamageSpec);
};
