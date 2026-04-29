#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "HitFlashComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitFlashStateChangedSignature, bool, bFlashActive);

UCLASS(ClassGroup=(Presentation), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UHitFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitFlashComponent();

	UPROPERTY(BlueprintAssignable, Category = "Presentation")
	FHitFlashStateChangedSignature OnFlashStateChanged;

	UFUNCTION(BlueprintPure, Category = "Presentation")
	bool IsFlashActive() const { return bFlashActive; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Presentation")
	float FlashDuration = 0.1f;

	UPROPERTY(VisibleAnywhere, Category = "Presentation")
	bool bFlashActive = false;

	FTimerHandle FlashTimerHandle;

	UFUNCTION()
	void HandleDamaged(UHealthComponent* HealthComponent, const FDamageSpec& DamageSpec);

	void EndFlash();
};
