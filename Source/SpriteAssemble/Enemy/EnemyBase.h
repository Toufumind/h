#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "../Core/GameplayInterfaces.h"
#include "EnemyBase.generated.h"

class UAttributeComponent;
class UCombatComponent;
class UEffectComponent;
class UEnemyActionComponent;
class UEnemyBrainComponent;
class UEnemyDefinition;
class UHealthComponent;
class UShooterComponent;
class UStaticMeshComponent;

UCLASS()
class SPRITEASSEMBLE_API AEnemyBase : public APaperCharacter, public IDamageable, public ITargetable, public ITeamAgent
{
	GENERATED_BODY()

public:
	AEnemyBase();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Enemy")
	void ApplyDefinition(UEnemyDefinition* InDefinition);

	virtual FGameplayCommandResult ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec) override;
	virtual bool IsTargetable_Implementation() const override;
	virtual EGameplayTeam GetGameplayTeam_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAttributeComponent> AttributeComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCombatComponent> CombatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEffectComponent> EffectComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UShooterComponent> ShooterComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEnemyBrainComponent> BrainComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEnemyActionComponent> ActionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MarkerPlaneComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MarkerPlaneBackComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Enemy")
	TObjectPtr<UEnemyDefinition> Definition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Enemy")
	EGameplayTeam Team = EGameplayTeam::Enemy;

	UFUNCTION()
	void HandleDeath(UHealthComponent* InHealthComponent);
};
