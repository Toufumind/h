#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "DamageReactionComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInvincibilityChangedSignature, bool, bInvincible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageReactionSignature, const FDamageSpec&, DamageSpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathReactionSignature);

UCLASS(ClassGroup=(ThreeC), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UDamageReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageReactionComponent();

	UPROPERTY(BlueprintAssignable, Category = "3C|Damage")
	FInvincibilityChangedSignature OnInvincibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "3C|Damage")
	FDamageReactionSignature OnDamageReaction;

	UPROPERTY(BlueprintAssignable, Category = "3C|Damage")
	FDeathReactionSignature OnDeathReaction;

	UFUNCTION(BlueprintPure, Category = "3C|Damage")
	bool IsInvincible() const { return bInvincible; }

	UFUNCTION(BlueprintPure, Category = "3C|Damage")
	bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintCallable, Category = "3C|Damage")
	bool CanReceiveDamage() const;

	UFUNCTION(BlueprintCallable, Category = "3C|Damage")
	void ResetReactionState();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "3C|Damage")
	float InvincibilitySeconds = 0.55f;

	UPROPERTY(EditAnywhere, Category = "3C|Damage")
	float DefaultKnockbackStrength = 560.0f;

	UPROPERTY(EditAnywhere, Category = "3C|Damage")
	float KnockbackUpwardStrength = 160.0f;

	UPROPERTY(VisibleAnywhere, Category = "3C|Damage")
	bool bInvincible = false;

	UPROPERTY(VisibleAnywhere, Category = "3C|Damage")
	bool bDead = false;

	FTimerHandle InvincibilityTimerHandle;

	UFUNCTION()
	void HandleDamaged(UHealthComponent* HealthComponent, const FDamageSpec& DamageSpec);

	UFUNCTION()
	void HandleDeath(UHealthComponent* HealthComponent);

	void BeginInvincibility();
	void EndInvincibility();
	void ApplyKnockback(const FDamageSpec& DamageSpec);
};
