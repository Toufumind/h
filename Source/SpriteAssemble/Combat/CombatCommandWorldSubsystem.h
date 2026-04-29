#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "CombatCommandWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;

UCLASS()
class SPRITEASSEMBLE_API UCombatCommandWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	FDelegateHandle HealHandle;
	FDelegateHandle ApplyDamageHandle;

	FGameplayCommandResult HandleHeal(const FGameplayCommand& Command) const;
	FGameplayCommandResult HandleApplyDamage(const FGameplayCommand& Command) const;
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
};
