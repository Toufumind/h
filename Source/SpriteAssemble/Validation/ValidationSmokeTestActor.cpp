#include "ValidationSmokeTestActor.h"

#include "../Enemy/EnemyBase.h"
#include "../Reward/RewardManagerWorldSubsystem.h"
#include "../Reward/RewardPoolDefinition.h"
#include "../Shot/ShotInstancedMeshView.h"
#include "../SpriteAssembleCharacter.h"
#include "../ThreeC/DamageReactionComponent.h"
#include "../ThreeC/PlayerMotorComponent.h"
#include "../UI/RewardViewModelWorldSubsystem.h"
#include "../UI/RunViewModelWorldSubsystem.h"
#include "../UI/UICommandControllerComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

namespace
{
bool HasVisibleMarkerPlane(const AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	for (const UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (!StaticMeshComponent)
		{
			continue;
		}

		const bool bIsMarkerPlane = StaticMeshComponent->GetFName().ToString().Contains(TEXT("MarkerPlane"));
		if (bIsMarkerPlane && StaticMeshComponent->IsVisible() && !StaticMeshComponent->bHiddenInGame && StaticMeshComponent->GetStaticMesh())
		{
			return true;
		}
	}

	return false;
}
}

AValidationSmokeTestActor::AValidationSmokeTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AValidationSmokeTestActor::BeginPlay()
{
	Super::BeginPlay();

	if (bRunSmokeOnBeginPlay)
	{
		RunSmoke();
	}
}

// Editor/smoke bootstrap only. Production content must wire RewardPool through RunConfig.
bool AValidationSmokeTestActor::ConfigureRewardPool()
{
	if (!RewardPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("Validation smoke RewardPool is unset. Assign it on the level instance before running validation; validation will not inject a fallback pool."));
		return false;
	}

	if (URewardManagerWorldSubsystem* RewardManager = GetWorld() ? GetWorld()->GetSubsystem<URewardManagerWorldSubsystem>() : nullptr)
	{
		RewardManager->SetRewardPool(RewardPool);
		UE_LOG(LogTemp, Display, TEXT("Validation smoke configured RewardPool: %s"), *GetNameSafe(RewardPool));
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Validation smoke could not find RewardManagerWorldSubsystem; RewardPool was not configured."));
	return false;
}

void AValidationSmokeTestActor::TriggerSelectNode()
{
	if (UUICommandControllerComponent* CommandController = FindCommandController())
	{
		CommandController->SelectNode(NodeId);
		UE_LOG(LogTemp, Display, TEXT("Validation smoke selected node: %s"), *NodeId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Validation smoke could not find a UICommandControllerComponent."));
	}
}

void AValidationSmokeTestActor::TriggerClaimReward()
{
	if (UUICommandControllerComponent* CommandController = FindCommandController())
	{
		CommandController->ClaimReward(RewardOptionIndex);
		UE_LOG(LogTemp, Display, TEXT("Validation smoke claimed reward option: %d"), RewardOptionIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Validation smoke could not find a UICommandControllerComponent."));
	}
}

void AValidationSmokeTestActor::RunSmoke()
{
	ConfigureRewardPool();
	TriggerSelectNode();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			LogSnapshot(CaptureSnapshot());
		}));

		FTimerHandle ClaimRewardTimerHandle;
		World->GetTimerManager().SetTimer(ClaimRewardTimerHandle, this, &AValidationSmokeTestActor::ClaimRewardAndCaptureSnapshot, ClaimRewardDelaySeconds, false);
	}
}

FValidationSmokeSnapshot AValidationSmokeTestActor::CaptureSnapshot() const
{
	FValidationSmokeSnapshot Snapshot;
	Snapshot.CommandOwnerName = FindCommandOwner() ? FindCommandOwner()->GetFName() : NAME_None;

	if (!GetWorld())
	{
		return Snapshot;
	}

	for (TActorIterator<ASpriteAssembleCharacter> It(GetWorld()); It; ++It)
	{
		++Snapshot.PlayerCount;
		Snapshot.PlayerActorNames.Add(It->GetFName());

		if (const UPlayerMotorComponent* PlayerMotorComponent = It->FindComponentByClass<UPlayerMotorComponent>())
		{
			Snapshot.bPlayerHasPlayerMotorComponent = true;
			Snapshot.PlayerHorizontalSpeed = PlayerMotorComponent->GetHorizontalSpeed();
			Snapshot.bPlayerIsFalling = PlayerMotorComponent->IsFalling();
			Snapshot.bPlayerIsJumpBuffered = PlayerMotorComponent->IsJumpBuffered();
			Snapshot.bPlayerIsUsingCoyoteTime = PlayerMotorComponent->IsUsingCoyoteTime();
		}

		if (const UDamageReactionComponent* DamageReactionComponent = It->FindComponentByClass<UDamageReactionComponent>())
		{
			Snapshot.bPlayerHasDamageReactionComponent = true;
			Snapshot.bPlayerIsInvincible = DamageReactionComponent->IsInvincible();
			Snapshot.bPlayerIsDead = DamageReactionComponent->IsDead();
		}

		Snapshot.bPlayerHasVisibleMarkerPlane |= HasVisibleMarkerPlane(*It);
	}

	for (TActorIterator<AEnemyBase> It(GetWorld()); It; ++It)
	{
		++Snapshot.EnemyBaseCount;
		Snapshot.bEnemyHasVisibleMarkerPlane |= HasVisibleMarkerPlane(*It);
	}

	for (TActorIterator<AShotInstancedMeshView> It(GetWorld()); It; ++It)
	{
		Snapshot.bHasShotInstancedMeshView = true;
		break;
	}

	if (const URunViewModelWorldSubsystem* RunViewModel = GetWorld()->GetSubsystem<URunViewModelWorldSubsystem>())
	{
		const FRunViewSnapshot RunSnapshot = RunViewModel->GetSnapshot();
		if (const UEnum* RunPhaseEnum = StaticEnum<ERunPhase>())
		{
			Snapshot.RunPhase = RunPhaseEnum->GetNameStringByValue(static_cast<int64>(RunSnapshot.CurrentPhase));
		}
		else
		{
			Snapshot.RunPhase = TEXT("Unknown");
		}
		Snapshot.CompletedNodeCount = RunSnapshot.CompletedNodeCount;
		Snapshot.RedCore = RunSnapshot.RedCore;
		Snapshot.GemSlotCount = RunSnapshot.GemSlotCount;
	}

	if (const URewardViewModelWorldSubsystem* RewardViewModel = GetWorld()->GetSubsystem<URewardViewModelWorldSubsystem>())
	{
		const TArray<FRewardOptionViewSnapshot> RewardOptions = RewardViewModel->GetRewardOptions();
		Snapshot.RewardOptionsCount = RewardOptions.Num();
		for (const FRewardOptionViewSnapshot& RewardOption : RewardOptions)
		{
			Snapshot.RewardOptionNames.Add(RewardOption.RewardSpec.RewardId);
		}
	}

	return Snapshot;
}

void AValidationSmokeTestActor::ValidationRunSmoke()
{
	RunSmoke();
}

void AValidationSmokeTestActor::ValidationSelectNode()
{
	ConfigureRewardPool();
	TriggerSelectNode();
}

void AValidationSmokeTestActor::ValidationClaimReward()
{
	TriggerClaimReward();
}

void AValidationSmokeTestActor::ValidationSnapshot()
{
	LogSnapshot(CaptureSnapshot());
}

void AValidationSmokeTestActor::ClaimRewardAndCaptureSnapshot()
{
	TriggerClaimReward();

	if (UWorld* World = GetWorld())
	{
		FTimerHandle SnapshotTimerHandle;
		World->GetTimerManager().SetTimer(SnapshotTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			LogSnapshot(CaptureSnapshot());
		}), SnapshotDelaySeconds, false);
	}
}

UUICommandControllerComponent* AValidationSmokeTestActor::FindCommandController() const
{
	if (AActor* FoundOwner = FindCommandOwner())
	{
		return FoundOwner->FindComponentByClass<UUICommandControllerComponent>();
	}

	return nullptr;
}

AActor* AValidationSmokeTestActor::FindCommandOwner() const
{
	if (CommandOwner)
	{
		return CommandOwner;
	}

	if (!GetWorld())
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->FindComponentByClass<UUICommandControllerComponent>())
		{
			return *It;
		}
	}

	return nullptr;
}

void AValidationSmokeTestActor::LogSnapshot(const FValidationSmokeSnapshot& Snapshot) const
{
	FString PlayerNames;
	for (const FName& PlayerActorName : Snapshot.PlayerActorNames)
	{
		if (!PlayerNames.IsEmpty())
		{
			PlayerNames += TEXT(",");
		}
		PlayerNames += PlayerActorName.ToString();
	}

	FString RewardNames;
	for (const FName& RewardOptionName : Snapshot.RewardOptionNames)
	{
		if (!RewardNames.IsEmpty())
		{
			RewardNames += TEXT(",");
		}
		RewardNames += RewardOptionName.ToString();
	}

	UE_LOG(LogTemp, Display, TEXT("Validation snapshot: Players=%d [%s] EnemyBase=%d ShotInstancedMeshView=%s PlayerMotor=%s DamageReaction=%s PlayerMarkerPlane=%s EnemyMarkerPlane=%s PlayerHorizontalSpeed=%.2f PlayerFalling=%s PlayerJumpBuffered=%s PlayerCoyoteTime=%s PlayerInvincible=%s PlayerDead=%s CommandOwner=%s RunPhase=%s CompletedNodeCount=%d RedCore=%d GemSlotCount=%d RewardOptions=%d [%s]"),
		Snapshot.PlayerCount,
		*PlayerNames,
		Snapshot.EnemyBaseCount,
		Snapshot.bHasShotInstancedMeshView ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerHasPlayerMotorComponent ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerHasDamageReactionComponent ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerHasVisibleMarkerPlane ? TEXT("true") : TEXT("false"),
		Snapshot.bEnemyHasVisibleMarkerPlane ? TEXT("true") : TEXT("false"),
		Snapshot.PlayerHorizontalSpeed,
		Snapshot.bPlayerIsFalling ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerIsJumpBuffered ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerIsUsingCoyoteTime ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerIsInvincible ? TEXT("true") : TEXT("false"),
		Snapshot.bPlayerIsDead ? TEXT("true") : TEXT("false"),
		*Snapshot.CommandOwnerName.ToString(),
		*Snapshot.RunPhase,
		Snapshot.CompletedNodeCount,
		Snapshot.RedCore,
		Snapshot.GemSlotCount,
		Snapshot.RewardOptionsCount,
		*RewardNames);
}
