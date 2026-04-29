#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "EconomyServiceWorldSubsystem.generated.h"

UCLASS()
class SPRITEASSEMBLE_API UEconomyServiceWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Gameplay|Economy")
	bool CanAfford(int32 Price) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Economy")
	FGameplayCommand BuildSpendCommand(int32 Price) const;
};
