#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayCommand.h"
#include "../Core/GameplaySpecs.h"
#include "EffectTypes.h"
#include "EffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusEffectChangedSignature, UEffectComponent*, EffectComponent, const FStatusEffectSpec&, StatusEffectSpec);

class UHealthComponent;

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEffectComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Effect")
	FStatusEffectChangedSignature OnStatusEffectApplied;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Effect")
	FStatusEffectChangedSignature OnStatusEffectExpired;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Effect")
	FGameplayCommandResult ApplyStatusEffect(const FStatusEffectSpec& StatusEffectSpec);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Effect")
	bool HasStatusEffect(FGameplayTag EffectTag) const;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Effect")
	TArray<FActiveStatusEffect> ActiveStatusEffects;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Effect")
	float StatusTickInterval = 1.0f;

	void TickStatusEffect(FActiveStatusEffect& ActiveStatusEffect, float DeltaTime);
	void ApplyPeriodicDamage(const FActiveStatusEffect& ActiveStatusEffect);
	void ExpireStatusEffectAt(int32 Index);
	UHealthComponent* GetHealthComponent() const;
};
