#include "ShotWorldSubsystem.h"
#include "ShotInstancedMeshView.h"
#include "../Combat/CombatComponent.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "../Core/GameplayInterfaces.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"

void UShotWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UShotWorldSubsystem::Deinitialize()
{
	Shots.Reset();
	FreeIndices.Reset();
	GenerationsByIndex.Reset();
	MeshView = nullptr;
	Super::Deinitialize();
}

void UShotWorldSubsystem::Tick(float DeltaTime)
{
	UpdateShots(DeltaTime);
	RemoveDeadShots();
	SyncVisuals();
}

TStatId UShotWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UShotWorldSubsystem, STATGROUP_Tickables);
}

bool UShotWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

TArray<FShotId> UShotWorldSubsystem::SubmitShotSpawnSpec(const FShotSpawnSpec& SpawnSpec)
{
	TArray<FShotId> SpawnedShotIds;
	const int32 ShotCount = FMath::Max(0, SpawnSpec.ShotCount);
	SpawnedShotIds.Reserve(ShotCount);

	if (ShotCount == 0)
	{
		return SpawnedShotIds;
	}

	const FVector BaseDirection = SpawnSpec.Direction.IsNearlyZero() ? FVector::ForwardVector : SpawnSpec.Direction.GetSafeNormal();
	const float StartOffset = ShotCount > 1 ? -0.5f * SpawnSpec.SpreadAngle * static_cast<float>(ShotCount - 1) : 0.0f;

	for (int32 ShotOffsetIndex = 0; ShotOffsetIndex < ShotCount; ++ShotOffsetIndex)
	{
		const FShotId ShotId = AllocateShotId();
		FShotInstance* Shot = FindShot(ShotId);
		if (!Shot)
		{
			continue;
		}

		const float AngleOffset = StartOffset + static_cast<float>(ShotOffsetIndex) * SpawnSpec.SpreadAngle;
		const FVector Direction = FRotator(0.0f, AngleOffset, 0.0f).RotateVector(BaseDirection).GetSafeNormal();

		Shot->ShotId = ShotId;
		Shot->Position = SpawnSpec.SpawnTransform.GetLocation();
		Shot->PreviousPosition = Shot->Position;
		Shot->Direction = Direction;
		Shot->Speed = SpawnSpec.Speed;
		Shot->Radius = SpawnSpec.Radius;
		Shot->ElapsedTime = 0.0f;
		Shot->Lifetime = SpawnSpec.Lifetime;
		Shot->PierceCount = SpawnSpec.PierceCount;
		Shot->PostPierceDamageMultiplier = SpawnSpec.PostPierceDamageMultiplier;
		Shot->VisualType = SpawnSpec.VisualType;
		Shot->InstanceIndex = INDEX_NONE;
		Shot->bAlive = true;
		Shot->NextGemIndex = SpawnSpec.NextGemIndex;
		Shot->OwnerActor = SpawnSpec.OwnerActor;
		Shot->InstigatorActor = SpawnSpec.InstigatorActor;
		Shot->Team = SpawnSpec.Team;
		Shot->DamageSpec = SpawnSpec.DamageSpec;

		SpawnedShotIds.Add(ShotId);
	}

	return SpawnedShotIds;
}

bool UShotWorldSubsystem::IsShotAlive(FShotId ShotId) const
{
	const FShotInstance* Shot = FindShot(ShotId);
	return Shot && Shot->bAlive;
}

void UShotWorldSubsystem::KillShot(FShotId ShotId)
{
	if (FShotInstance* Shot = FindShot(ShotId))
	{
		Shot->bAlive = false;
	}
}

FShotId UShotWorldSubsystem::AllocateShotId()
{
	int32 Index = INDEX_NONE;
	if (FreeIndices.Num() > 0)
	{
		Index = FreeIndices.Pop(EAllowShrinking::No);
	}
	else
	{
		Index = Shots.AddDefaulted();
	}

	int32& Generation = GenerationsByIndex.FindOrAdd(Index);
	++Generation;

	FShotId ShotId;
	ShotId.Index = Index;
	ShotId.Generation = Generation;
	return ShotId;
}

FShotInstance* UShotWorldSubsystem::FindShot(FShotId ShotId)
{
	if (!ShotId.IsValid() || !Shots.IsValidIndex(ShotId.Index))
	{
		return nullptr;
	}

	const int32* Generation = GenerationsByIndex.Find(ShotId.Index);
	if (!Generation || *Generation != ShotId.Generation)
	{
		return nullptr;
	}

	return &Shots[ShotId.Index];
}

const FShotInstance* UShotWorldSubsystem::FindShot(FShotId ShotId) const
{
	if (!ShotId.IsValid() || !Shots.IsValidIndex(ShotId.Index))
	{
		return nullptr;
	}

	const int32* Generation = GenerationsByIndex.Find(ShotId.Index);
	if (!Generation || *Generation != ShotId.Generation)
	{
		return nullptr;
	}

	return &Shots[ShotId.Index];
}

void UShotWorldSubsystem::UpdateShots(float DeltaTime)
{
	for (FShotInstance& Shot : Shots)
	{
		if (!Shot.bAlive)
		{
			continue;
		}

		Shot.ElapsedTime += DeltaTime;
		if (Shot.ElapsedTime >= Shot.Lifetime)
		{
			Shot.bAlive = false;
			continue;
		}

		Shot.PreviousPosition = Shot.Position;
		Shot.Position += Shot.Direction * Shot.Speed * DeltaTime;
		ProcessShotCollision(Shot);
	}
}

void UShotWorldSubsystem::ProcessShotCollision(FShotInstance& Shot)
{
	UWorld* World = GetWorld();
	if (!World || !Shot.bAlive)
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ShotWorldSubsystem), false);
	QueryParams.AddIgnoredActor(Shot.OwnerActor);
	QueryParams.AddIgnoredActor(Shot.InstigatorActor);

	TArray<FHitResult> HitResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(FMath::Max(1.0f, Shot.Radius));
	World->SweepMultiByChannel(HitResults, Shot.PreviousPosition, Shot.Position, FQuat::Identity, ECC_Pawn, CollisionShape, QueryParams);

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!ShouldHitActor(Shot, HitActor))
		{
			continue;
		}

		ResolveShotHit(Shot, HitActor, HitResult);
		if (!Shot.bAlive)
		{
			return;
		}
	}
}

bool UShotWorldSubsystem::ShouldHitActor(const FShotInstance& Shot, AActor* Candidate) const
{
	if (!Candidate || Candidate == Shot.OwnerActor || Candidate == Shot.InstigatorActor)
	{
		return false;
	}

	if (Candidate->GetClass()->ImplementsInterface(UTeamAgent::StaticClass()) &&
		ITeamAgent::Execute_GetGameplayTeam(Candidate) == Shot.Team)
	{
		return false;
	}

	if (Candidate->GetClass()->ImplementsInterface(UTargetable::StaticClass()) &&
		!ITargetable::Execute_IsTargetable(Candidate))
	{
		return false;
	}

	return Candidate->GetClass()->ImplementsInterface(UDamageable::StaticClass()) ||
		Candidate->FindComponentByClass<UCombatComponent>() != nullptr;
}

void UShotWorldSubsystem::ResolveShotHit(FShotInstance& Shot, AActor* TargetActor, const FHitResult& HitResult)
{
	FShotHitSpec HitSpec;
	HitSpec.ShotId = Shot.ShotId;
	HitSpec.SourceActor = Shot.OwnerActor;
	HitSpec.InstigatorActor = Shot.InstigatorActor;
	HitSpec.TargetActor = TargetActor;
	HitSpec.HitLocation = HitResult.ImpactPoint.IsNearlyZero() ? Shot.Position : FVector(HitResult.ImpactPoint);
	HitSpec.HitNormal = HitResult.ImpactNormal.IsNearlyZero() ? -Shot.Direction : FVector(HitResult.ImpactNormal);
	HitSpec.NextGemIndex = Shot.NextGemIndex;
	HitSpec.DamageSpec = Shot.DamageSpec;
	HitSpec.Tags = Shot.DamageSpec.Tags;

	TArray<FGameplayCommand> GeneratedCommands;
	if (AActor* SourceActor = Shot.OwnerActor.Get())
	{
		TArray<UActorComponent*> Consumers = SourceActor->GetComponentsByInterface(UShotHitSpecConsumer::StaticClass());
		for (UActorComponent* Consumer : Consumers)
		{
			if (Consumer)
			{
				IShotHitSpecConsumer::Execute_ConsumeShotHitSpec(Consumer, HitSpec, GeneratedCommands);
			}
		}
	}

	FGameplayCommandResult ResolveResult = FGameplayCommandResult::Unhandled();
	if (AActor* SourceActor = Shot.OwnerActor.Get())
	{
		if (UCombatComponent* CombatComponent = SourceActor->FindComponentByClass<UCombatComponent>())
		{
			ResolveResult = CombatComponent->ResolveShotHit(HitSpec);
		}
	}

	DispatchGeneratedCommands(GeneratedCommands);

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::ShotHit);
		Event.SourceActor = Shot.OwnerActor;
		Event.TargetActor = TargetActor;
		Event.ShotId = Shot.ShotId;
		Event.ShotHitSpec = HitSpec;
		Event.DamageSpec = HitSpec.DamageSpec;
		EventBus->QueueEvent(Event);
	}

	if (ResolveResult.bHandled && ResolveResult.bSucceeded)
	{
		Shot.DamageSpec.FinalDamage *= Shot.PostPierceDamageMultiplier;
	}

	if (Shot.PierceCount <= 0)
	{
		Shot.bAlive = false;
		return;
	}

	--Shot.PierceCount;
}

void UShotWorldSubsystem::DispatchGeneratedCommands(const TArray<FGameplayCommand>& Commands) const
{
	if (Commands.Num() == 0)
	{
		return;
	}

	UGameplayCommandDispatcher* CommandDispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
	if (!CommandDispatcher)
	{
		return;
	}

	for (const FGameplayCommand& Command : Commands)
	{
		CommandDispatcher->QueueCommand(Command);
	}
	CommandDispatcher->FlushQueuedCommands();
}

void UShotWorldSubsystem::RemoveDeadShots()
{
	for (int32 Index = 0; Index < Shots.Num(); ++Index)
	{
		FShotInstance& Shot = Shots[Index];
		if (!Shot.bAlive && Shot.ShotId.IsValid())
		{
			Shot.ShotId.Reset();
			Shot.InstanceIndex = INDEX_NONE;
			FreeIndices.AddUnique(Index);
		}
	}
}

void UShotWorldSubsystem::SyncVisuals()
{
	EnsureMeshView();
	if (!MeshView)
	{
		return;
	}

	MeshView->ClearShotVisuals();
	for (FShotInstance& Shot : Shots)
	{
		if (!Shot.bAlive)
		{
			continue;
		}

		Shot.InstanceIndex = MeshView->AddShotVisual(Shot);
	}
}

void UShotWorldSubsystem::EnsureMeshView()
{
	if (MeshView || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Name = TEXT("ShotInstancedMeshView");
	SpawnParameters.ObjectFlags |= RF_Transient;
	MeshView = GetWorld()->SpawnActor<AShotInstancedMeshView>(AShotInstancedMeshView::StaticClass(), FTransform::Identity, SpawnParameters);
}
