#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayIds.h"
#include "../Core/GameplaySpecs.h"
#include "ShooterComponent.generated.h"

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UShooterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShooterComponent();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Weapon")
	FShotSpawnSpec BuildShotSpawnSpec(AActor* TargetActor, const FDamageSpec& DamageSpec, float Speed, float Radius, float Lifetime, FName VisualType, EGameplayTeam Team) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Weapon")
	TArray<FShotId> SubmitShotSpawnSpec(const FShotSpawnSpec& ShotSpawnSpec) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Weapon")
	TArray<FShotId> FireAtTarget(AActor* TargetActor, const FDamageSpec& DamageSpec, float Speed, float Radius, float Lifetime, FName VisualType, EGameplayTeam Team) const;
};
