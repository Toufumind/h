#include "DamageResolver.h"
#include "HealthComponent.h"

FGameplayCommandResult FDamageResolver::ApplyDamage(AActor* TargetActor, const FDamageSpec& DamageSpec)
{
	if (!TargetActor)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "MissingDamageTarget", "Missing damage target."));
	}

	UHealthComponent* HealthComponent = TargetActor->FindComponentByClass<UHealthComponent>();
	if (!HealthComponent)
	{
		return FGameplayCommandResult::Unhandled();
	}

	return HealthComponent->ApplyDamageSpec(DamageSpec);
}
