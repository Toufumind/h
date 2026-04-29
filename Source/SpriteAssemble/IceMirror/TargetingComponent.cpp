#include "TargetingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/GameplayInterfaces.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FTargetingResult UTargetingComponent::FindNearestTarget(float Range, EGameplayTeam OwnerTeam) const
{
	FTargetingResult Result;
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !GetWorld())
	{
		return Result;
	}

	TArray<AActor*> CandidateActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UTargetable::StaticClass(), CandidateActors);

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const float RangeSquared = FMath::Square(Range);
	float BestDistanceSquared = TNumericLimits<float>::Max();

	for (AActor* Candidate : CandidateActors)
	{
		if (!IsValidTarget(Candidate, OwnerTeam))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(OwnerLocation, Candidate->GetActorLocation());
		if (DistanceSquared > RangeSquared || DistanceSquared >= BestDistanceSquared)
		{
			continue;
		}

		BestDistanceSquared = DistanceSquared;
		Result.TargetActor = Candidate;
	}

	if (Result.TargetActor)
	{
		Result.Distance = FMath::Sqrt(BestDistanceSquared);
		Result.Direction = (Result.TargetActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
	}

	return Result;
}

bool UTargetingComponent::IsValidTarget(AActor* Candidate, EGameplayTeam OwnerTeam) const
{
	if (!Candidate || Candidate == GetOwner())
	{
		return false;
	}

	if (Candidate->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
	{
		const bool bTargetable = ITargetable::Execute_IsTargetable(Candidate);
		if (!bTargetable)
		{
			return false;
		}
	}

	if (Candidate->GetClass()->ImplementsInterface(UTeamAgent::StaticClass()))
	{
		const EGameplayTeam CandidateTeam = ITeamAgent::Execute_GetGameplayTeam(Candidate);
		if (CandidateTeam == OwnerTeam)
		{
			return false;
		}
	}

	return true;
}
