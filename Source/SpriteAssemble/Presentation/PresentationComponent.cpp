#include "PresentationComponent.h"

UPresentationComponent::UPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPresentationComponent::SetFacingDirection(FVector InFacingDirection)
{
	if (!InFacingDirection.IsNearlyZero())
	{
		PresentationState.FacingDirection = InFacingDirection.GetSafeNormal();
	}
}

void UPresentationComponent::AddStateTag(FGameplayTag StateTag)
{
	if (StateTag.IsValid())
	{
		PresentationState.StateTags.AddTag(StateTag);
	}
}

void UPresentationComponent::RemoveStateTag(FGameplayTag StateTag)
{
	if (StateTag.IsValid())
	{
		PresentationState.StateTags.RemoveTag(StateTag);
	}
}

FPresentationState UPresentationComponent::GetPresentationState_Implementation() const
{
	return PresentationState;
}
