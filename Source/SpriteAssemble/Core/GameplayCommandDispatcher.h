#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayCommand.h"
#include "GameplayCommandDispatcher.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(FGameplayCommandResult, FGameplayCommandNativeHandler, const FGameplayCommand&);

UCLASS()
class SPRITEASSEMBLE_API UGameplayCommandDispatcher : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void QueueCommand(const FGameplayCommand& Command);
	FGameplayCommandResult DispatchImmediate(const FGameplayCommand& Command) const;
	TArray<FGameplayCommandResult> FlushQueuedCommands();

	FDelegateHandle RegisterHandler(EGameplayCommandType Type, FGameplayCommandNativeHandler Handler);
	void UnregisterHandler(EGameplayCommandType Type, FDelegateHandle Handle);

private:
	struct FRegisteredCommandHandler
	{
		FDelegateHandle Handle;
		FGameplayCommandNativeHandler Handler;
	};

	TArray<FGameplayCommand> QueuedCommands;
	TMap<EGameplayCommandType, TArray<FRegisteredCommandHandler>> HandlersByType;
};
