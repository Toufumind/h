#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplaySpecs.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHitConfirmedSignature, UCombatComponent*, CombatComponent, const FShotHitSpec&, HitSpec);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Combat")
	FHitConfirmedSignature OnHitConfirmed;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Combat")
	FAttackSpec BuildAttackSpec(AActor* TargetActor, float BaseDamage, float Range, const FVector& Direction) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Combat")
	FDamageSpec BuildDamageSpec(const FAttackSpec& AttackSpec) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Combat")
	FGameplayCommandResult ResolveShotHit(FShotHitSpec& HitSpec);
};
