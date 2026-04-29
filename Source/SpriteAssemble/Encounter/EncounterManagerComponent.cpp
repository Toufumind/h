#include "EncounterManagerComponent.h"
#include "EncounterConfig.h"
#include "SpawnDirector.h"
#include "WaveConfig.h"
#include "WaveDirector.h"
#include "../Core/GameplayInterfaces.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "Engine/World.h"

UEncounterManagerComponent::UEncounterManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEncounterManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	WaveDirector = NewObject<UWaveDirector>(this);
	SpawnDirector = NewObject<USpawnDirector>(this);

	if (UGameplayCommandDispatcher* Dispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr)
	{
		StartEncounterHandle = Dispatcher->RegisterHandler(EGameplayCommandType::StartEncounter, FGameplayCommandNativeHandler::CreateUObject(this, &UEncounterManagerComponent::HandleStartEncounterCommand));
	}
}

void UEncounterManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameplayCommandDispatcher* Dispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr)
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::StartEncounter, StartEncounterHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UEncounterManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bRunning || !WaveDirector || !SpawnDirector)
	{
		return;
	}

	TArray<AActor*> SpawnedEnemies;
	WaveDirector->TickWave(DeltaTime, GetWorld(), SpawnPoints, SpawnDirector, SpawnedEnemies);
	for (AActor* Enemy : SpawnedEnemies)
	{
		RegisterSpawnedEnemy(Enemy);
	}

	PruneDeadEnemies();
	if (IsCurrentWaveCleared())
	{
		StartNextWave();
	}
}

void UEncounterManagerComponent::StartEncounter(UEncounterConfig* InEncounterConfig)
{
	EncounterConfig = InEncounterConfig;
	AliveEnemies.Reset();
	CurrentWaveIndex = INDEX_NONE;
	bEncounterCompleted = false;
	bRunning = EncounterConfig != nullptr;

	if (bRunning)
	{
		StartNextWave();
	}
}

FGameplayCommandResult UEncounterManagerComponent::HandleStartEncounterCommand(const FGameplayCommand& Command)
{
	UEncounterConfig* ConfigToStart = DefaultEncounterConfig ? DefaultEncounterConfig.Get() : EncounterConfig.Get();
	if (!ConfigToStart)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "EncounterMissingConfig", "Encounter config is missing."));
	}

	StartEncounter(ConfigToStart);
	return FGameplayCommandResult::Success();
}

void UEncounterManagerComponent::RegisterSpawnPoint(AActor* SpawnPoint)
{
	if (SpawnPoint)
	{
		SpawnPoints.AddUnique(SpawnPoint);
	}
}

void UEncounterManagerComponent::ClearSpawnPoints()
{
	SpawnPoints.Reset();
}

void UEncounterManagerComponent::StartNextWave()
{
	if (!EncounterConfig || !WaveDirector)
	{
		CompleteEncounter();
		return;
	}

	++CurrentWaveIndex;
	if (!EncounterConfig->Waves.IsValidIndex(CurrentWaveIndex))
	{
		CompleteEncounter();
		return;
	}

	WaveDirector->StartWave(EncounterConfig->Waves[CurrentWaveIndex]);
}

void UEncounterManagerComponent::RegisterSpawnedEnemy(AActor* Enemy)
{
	if (Enemy)
	{
		AliveEnemies.AddUnique(Enemy);
	}
}

void UEncounterManagerComponent::PruneDeadEnemies()
{
	for (int32 Index = AliveEnemies.Num() - 1; Index >= 0; --Index)
	{
		AActor* Enemy = AliveEnemies[Index].Get();
		const bool bStillTargetable = Enemy &&
			Enemy->GetClass()->ImplementsInterface(UTargetable::StaticClass()) &&
			ITargetable::Execute_IsTargetable(Enemy);

		if (!Enemy || Enemy->IsActorBeingDestroyed() || !bStillTargetable)
		{
			AliveEnemies.RemoveAtSwap(Index);
		}
	}
}

bool UEncounterManagerComponent::IsCurrentWaveCleared() const
{
	return WaveDirector && WaveDirector->HasSpawnedEverything() && AliveEnemies.Num() == 0;
}

void UEncounterManagerComponent::CompleteEncounter()
{
	if (!bRunning || bEncounterCompleted)
	{
		return;
	}

	bRunning = false;
	bEncounterCompleted = true;
	OnEncounterCompleted.Broadcast(this);

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::EncounterCompleted);
		Event.SourceActor = GetOwner();
		Event.NameValue = EncounterConfig ? EncounterConfig->EncounterId : NAME_None;
		EventBus->QueueEvent(Event);
	}
}
