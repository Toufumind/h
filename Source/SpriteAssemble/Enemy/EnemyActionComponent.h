#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "EnemyBrainComponent.h"
#include "EnemyActionComponent.generated.h"

class UEnemyActionDefinition;
class UShooterComponent;

UENUM(BlueprintType)
enum class EEnemyActionPhase : uint8
{
	Ready,
	Telegraph,
	Startup,
	Active,
	Recovery,
	Cooldown
};

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UEnemyActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyActionComponent();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Enemy")
	void Configure(float InMoveSpeed, float InContactDamage, float InAttackCooldown, EGameplayTeam InTeam, UEnemyActionDefinition* InDefaultAction);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Enemy")
	EEnemyActionPhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Gameplay|Enemy")
	bool IsInAction() const { return CurrentPhase != EEnemyActionPhase::Ready && CurrentPhase != EEnemyActionPhase::Cooldown; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float ContactDamage = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float TelegraphTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float StartupTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float ActiveTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float RecoveryTime = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float AttackRange = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	bool bUseShot = false;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float ShotSpeed = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float ShotRadius = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float ShotLifetime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	FName ShotVisualType = TEXT("EnemyDefault");

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	EGameplayTeam Team = EGameplayTeam::Enemy;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Enemy")
	EEnemyActionPhase CurrentPhase = EEnemyActionPhase::Ready;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Enemy")
	float PhaseRemainingTime = 0.0f;

	UPROPERTY()
	TWeakObjectPtr<AActor> LockedTarget;

	bool bHasAppliedActiveHit = false;

	UPROPERTY()
	TObjectPtr<UEnemyBrainComponent> BrainComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UShooterComponent> ShooterComponent = nullptr;

	void ChaseTarget(AActor* Target, float DeltaTime);
	void TryBeginAttack(AActor* Target);
	void TickActionPhase(float DeltaTime);
	void AdvanceActionPhase();
	void EnterPhase(EEnemyActionPhase NewPhase, float Duration);
	void ApplyActiveHit();
	void ApplyMeleeHit(AActor* Target);
	void FireShot(AActor* Target);
	FDamageSpec BuildDamageSpec() const;
	bool IsTargetInRange(AActor* Target) const;
};
