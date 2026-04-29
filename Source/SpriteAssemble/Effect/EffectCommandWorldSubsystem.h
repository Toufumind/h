#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "EffectCommandWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;

UCLASS()
class SPRITEASSEMBLE_API UEffectCommandWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	FDelegateHandle ApplyStatusEffectHandle;

	FGameplayCommandResult HandleApplyStatusEffect(const FGameplayCommand& Command) const;
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
};
