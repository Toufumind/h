#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "IceMirrorComponent.generated.h"

class UIceMirrorDefinition;
class UTargetingComponent;
class UCombatComponent;

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UIceMirrorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UIceMirrorComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|IceMirror")
	void StartAutoCast();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|IceMirror")
	void StopAutoCast();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|IceMirror")
	void TryCastOnce();

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|IceMirror")
	TObjectPtr<UIceMirrorDefinition> Definition;

	UPROPERTY(EditAnywhere, Category = "Gameplay|IceMirror")
	EGameplayTeam Team = EGameplayTeam::Player;

	UPROPERTY(EditAnywhere, Category = "Gameplay|IceMirror")
	bool bAutoStart = true;

	FTimerHandle AutoCastTimerHandle;

	float GetAttackInterval() const;
	float GetRange() const;
	float GetBaseDamage() const;
	float GetShotSpeed() const;
	float GetShotRadius() const;
	float GetShotLifetime() const;
	FName GetShotVisualType() const;

	UTargetingComponent* GetTargetingComponent() const;
	UCombatComponent* GetCombatComponent() const;
};
