#include "ShotImpactPresenterWorldSubsystem.h"
#include "../Core/GameplayEventBus.h"

void UShotImpactPresenterWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		ShotHitHandle = EventBus->Subscribe(EGameplayEventType::ShotHit, FGameplayNativeEvent::FDelegate::CreateUObject(this, &UShotImpactPresenterWorldSubsystem::HandleShotHit));
	}
}

void UShotImpactPresenterWorldSubsystem::Deinitialize()
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		EventBus->Unsubscribe(EGameplayEventType::ShotHit, ShotHitHandle);
	}

	Super::Deinitialize();
}

void UShotImpactPresenterWorldSubsystem::HandleShotHit(const FGameplayEvent& Event)
{
	OnShotImpactPresented.Broadcast(Event.ShotHitSpec);
}
