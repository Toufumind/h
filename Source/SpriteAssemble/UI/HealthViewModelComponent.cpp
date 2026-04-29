#include "HealthViewModelComponent.h"
#include "../Combat/HealthComponent.h"

UHealthViewModelComponent::UHealthViewModelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthViewModelComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr;
	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &UHealthViewModelComponent::HandleHealthChanged);
		HealthComponent->OnDeath.AddDynamic(this, &UHealthViewModelComponent::HandleDeath);
	}

	RefreshSnapshot();
}

void UHealthViewModelComponent::HandleHealthChanged(UHealthComponent* InHealthComponent, float CurrentHealth, float MaxHealth)
{
	Snapshot.CurrentHealth = CurrentHealth;
	Snapshot.MaxHealth = MaxHealth;
	Snapshot.bDead = InHealthComponent ? InHealthComponent->IsDead() : Snapshot.bDead;
	OnSnapshotChanged.Broadcast(Snapshot);
}

void UHealthViewModelComponent::HandleDeath(UHealthComponent* InHealthComponent)
{
	RefreshSnapshot();
}

void UHealthViewModelComponent::RefreshSnapshot()
{
	if (HealthComponent)
	{
		Snapshot.CurrentHealth = HealthComponent->GetCurrentHealth();
		Snapshot.MaxHealth = HealthComponent->GetMaxHealth();
		Snapshot.bDead = HealthComponent->IsDead();
	}

	OnSnapshotChanged.Broadcast(Snapshot);
}
