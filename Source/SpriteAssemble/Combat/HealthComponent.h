#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "../Core/GameplayCommand.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealthChangedSignature, UHealthComponent*, HealthComponent, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamagedSignature, UHealthComponent*, HealthComponent, const FDamageSpec&, DamageSpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathSignature, UHealthComponent*, HealthComponent);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Health")
	FHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Health")
	FDamagedSignature OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Health")
	FDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Health")
	void InitializeHealth(float InMaxHealth);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Health")
	FGameplayCommandResult ApplyDamageSpec(const FDamageSpec& DamageSpec);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Health")
	FGameplayCommandResult Heal(float HealAmount);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Gameplay|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Gameplay|Health")
	bool IsDead() const { return bIsDead; }

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Health")
	bool bIsDead = false;

	void BroadcastHealthChanged();
	void Die();
};
