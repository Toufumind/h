#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "EnemyBrainComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemyIntent : uint8
{
	Idle,
	Chase,
	Attack,
	Dead
};

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UEnemyBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyBrainComponent();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Enemy")
	void SetTeam(EGameplayTeam InTeam);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Enemy")
	void SetRanges(float InDetectionRange, float InAttackRange);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Enemy")
	void SetDead(bool bInDead);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Enemy")
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintPure, Category = "Gameplay|Enemy")
	EEnemyIntent GetCurrentIntent() const { return CurrentIntent; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Enemy")
	EEnemyIntent CurrentIntent = EEnemyIntent::Idle;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	EGameplayTeam Team = EGameplayTeam::Enemy;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float DetectionRange = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Enemy")
	float AttackRange = 80.0f;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay|Enemy")
	bool bDead = false;

	void UpdateIntent();
	AActor* FindNearestTarget() const;
};
