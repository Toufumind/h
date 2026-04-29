#include "RunViewModelWorldSubsystem.h"
#include "../Run/RunStateWorldSubsystem.h"

FRunViewSnapshot URunViewModelWorldSubsystem::GetSnapshot() const
{
	FRunViewSnapshot Snapshot;
	const URunStateWorldSubsystem* RunStateSubsystem = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr;
	if (!RunStateSubsystem)
	{
		return Snapshot;
	}

	const FRunState& RunState = RunStateSubsystem->GetRunState();
	Snapshot.CurrentPhase = RunState.CurrentPhase;
	Snapshot.RedCore = RunState.RedCore;
	Snapshot.CompletedNodeCount = RunState.CompletedNodeCount;
	Snapshot.GemSlotCount = RunState.GemSlotCount;
	Snapshot.PotionMax = RunState.PotionMax;
	return Snapshot;
}

void URunViewModelWorldSubsystem::Refresh()
{
	OnSnapshotChanged.Broadcast(GetSnapshot());
}
