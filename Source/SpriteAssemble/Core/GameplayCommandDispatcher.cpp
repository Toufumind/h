#include "GameplayCommandDispatcher.h"

void UGameplayCommandDispatcher::QueueCommand(const FGameplayCommand& Command)
{
	QueuedCommands.Add(Command);
}

FGameplayCommandResult UGameplayCommandDispatcher::DispatchImmediate(const FGameplayCommand& Command) const
{
	const TArray<FRegisteredCommandHandler>* Handlers = HandlersByType.Find(Command.Type);
	if (!Handlers)
	{
		return FGameplayCommandResult::Unhandled();
	}

	for (const FRegisteredCommandHandler& RegisteredHandler : *Handlers)
	{
		if (RegisteredHandler.Handler.IsBound())
		{
			FGameplayCommandResult Result = RegisteredHandler.Handler.Execute(Command);
			if (Result.bHandled)
			{
				return Result;
			}
		}
	}

	return FGameplayCommandResult::Unhandled();
}

TArray<FGameplayCommandResult> UGameplayCommandDispatcher::FlushQueuedCommands()
{
	TArray<FGameplayCommand> CommandsToProcess = MoveTemp(QueuedCommands);
	QueuedCommands.Reset();

	TArray<FGameplayCommandResult> Results;
	Results.Reserve(CommandsToProcess.Num());

	for (const FGameplayCommand& Command : CommandsToProcess)
	{
		Results.Add(DispatchImmediate(Command));
	}

	return Results;
}

FDelegateHandle UGameplayCommandDispatcher::RegisterHandler(EGameplayCommandType Type, FGameplayCommandNativeHandler Handler)
{
	FRegisteredCommandHandler RegisteredHandler;
	RegisteredHandler.Handle = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
	RegisteredHandler.Handler = MoveTemp(Handler);

	FDelegateHandle Handle = RegisteredHandler.Handle;
	HandlersByType.FindOrAdd(Type).Add(MoveTemp(RegisteredHandler));
	return Handle;
}

void UGameplayCommandDispatcher::UnregisterHandler(EGameplayCommandType Type, FDelegateHandle Handle)
{
	if (TArray<FRegisteredCommandHandler>* Handlers = HandlersByType.Find(Type))
	{
		Handlers->RemoveAll([Handle](const FRegisteredCommandHandler& RegisteredHandler)
		{
			return RegisteredHandler.Handle == Handle;
		});
	}
}
