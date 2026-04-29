#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "GemCommandWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;

UCLASS()
class SPRITEASSEMBLE_API UGemCommandWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	FDelegateHandle AddGemHandle;
	FDelegateHandle EquipGemHandle;
	FDelegateHandle ReorderGemsHandle;

	FGameplayCommandResult HandleAddGem(const FGameplayCommand& Command) const;
	FGameplayCommandResult HandleEquipGem(const FGameplayCommand& Command) const;
	FGameplayCommandResult HandleReorderGems(const FGameplayCommand& Command) const;
	UGameplayCommandDispatcher* GetCommandDispatcher() const;
};
