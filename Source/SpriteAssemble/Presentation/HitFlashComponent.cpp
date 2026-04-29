#include "HitFlashComponent.h"
#include "../Combat/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UHitFlashComponent::UHitFlashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHitFlashComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthComponent* HealthComponent = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr)
	{
		HealthComponent->OnDamaged.AddDynamic(this, &UHitFlashComponent::HandleDamaged);
	}
}

void UHitFlashComponent::HandleDamaged(UHealthComponent* HealthComponent, const FDamageSpec& DamageSpec)
{
	bFlashActive = true;
	OnFlashStateChanged.Broadcast(bFlashActive);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(FlashTimerHandle, this, &UHitFlashComponent::EndFlash, FlashDuration, false);
	}
}

void UHitFlashComponent::EndFlash()
{
	bFlashActive = false;
	OnFlashStateChanged.Broadcast(bFlashActive);
}
