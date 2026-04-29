#include "EncounterSpawnPointComponent.h"
#include "EncounterManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UEncounterSpawnPointComponent::UEncounterSpawnPointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEncounterSpawnPointComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (!SpawnPointTag.IsNone())
	{
		Owner->Tags.AddUnique(SpawnPointTag);
	}

	if (bRegisterOnBeginPlay)
	{
		if (UEncounterManagerComponent* EncounterManager = FindEncounterManager())
		{
			EncounterManager->RegisterSpawnPoint(Owner);
		}
	}
}

UEncounterManagerComponent* UEncounterSpawnPointComponent::FindEncounterManager() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (UEncounterManagerComponent* EncounterManager = Actor ? Actor->FindComponentByClass<UEncounterManagerComponent>() : nullptr)
		{
			return EncounterManager;
		}
	}

	return nullptr;
}
