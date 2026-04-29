#include "EconomyServiceWorldSubsystem.h"
#include "../Run/RunStateWorldSubsystem.h"

bool UEconomyServiceWorldSubsystem::CanAfford(int32 Price) const
{
	const URunStateWorldSubsystem* RunState = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr;
	return RunState && RunState->GetRunState().RedCore >= FMath::Max(0, Price);
}

FGameplayCommand UEconomyServiceWorldSubsystem::BuildSpendCommand(int32 Price) const
{
	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::SpendRedCore;
	Command.IntValue = FMath::Max(0, Price);
	return Command;
}
