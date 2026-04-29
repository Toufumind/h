#include "EnemyBrainComponent.h"
#include "../Core/GameplayInterfaces.h"
#include "Kismet/GameplayStatics.h"

UEnemyBrainComponent::UEnemyBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyBrainComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateIntent();
}

void UEnemyBrainComponent::SetTeam(EGameplayTeam InTeam)
{
	Team = InTeam;
}

void UEnemyBrainComponent::SetRanges(float InDetectionRange, float InAttackRange)
{
	DetectionRange = InDetectionRange;
	AttackRange = InAttackRange;
}

void UEnemyBrainComponent::SetDead(bool bInDead)
{
	bDead = bInDead;
	if (bDead)
	{
		CurrentIntent = EEnemyIntent::Dead;
		CurrentTarget.Reset();
	}
}

void UEnemyBrainComponent::UpdateIntent()
{
	if (bDead)
	{
		CurrentIntent = EEnemyIntent::Dead;
		return;
	}

	CurrentTarget = FindNearestTarget();
	AActor* Owner = GetOwner();
	AActor* Target = CurrentTarget.Get();
	if (!Owner || !Target)
	{
		CurrentIntent = EEnemyIntent::Idle;
		return;
	}

	const float Distance = FVector::Dist2D(Owner->GetActorLocation(), Target->GetActorLocation());
	CurrentIntent = Distance <= AttackRange ? EEnemyIntent::Attack : EEnemyIntent::Chase;
}

AActor* UEnemyBrainComponent::FindNearestTarget() const
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner)
	{
		return nullptr;
	}

	TArray<AActor*> Candidates;
	UGameplayStatics::GetAllActorsWithInterface(World, UTargetable::StaticClass(), Candidates);

	AActor* BestTarget = nullptr;
	float BestDistanceSquared = DetectionRange * DetectionRange;

	for (AActor* Candidate : Candidates)
	{
		if (!Candidate || Candidate == Owner)
		{
			continue;
		}

		if (!ITargetable::Execute_IsTargetable(Candidate))
		{
			continue;
		}

		if (Candidate->GetClass()->ImplementsInterface(UTeamAgent::StaticClass()) &&
			ITeamAgent::Execute_GetGameplayTeam(Candidate) == Team)
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared2D(Owner->GetActorLocation(), Candidate->GetActorLocation());
		if (DistanceSquared <= BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}
