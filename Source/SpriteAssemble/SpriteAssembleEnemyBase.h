#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Core/GameplayInterfaces.h"
#include "SpriteAssembleEnemyBase.generated.h"

class UTextRenderComponent;
class UActorComponent;

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleEnemyBase : public APaperCharacter, public IDamageable, public ITargetable, public ITeamAgent
{
	GENERATED_BODY()

public:
	ASpriteAssembleEnemyBase();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGameplayCommandResult ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec) override;
	virtual bool IsTargetable_Implementation() const override;
	virtual EGameplayTeam GetGameplayTeam_Implementation() const override;

protected:
	UFUNCTION()
	void OnEnemyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ResetFlashColor();
	FTimerHandle FlashTimerHandle;

	virtual void BeginPlay() override;

	// 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* HealthTextComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UActorComponent* PaperZDAnimComponent;

	// 属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRadius = 800.0f; // 检测到玩家的距离

	// 动画状态（暴露给PaperZD使用）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsMoving = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAttacking = false;

	void UpdateHealthUI();
	void Die();

	// 获取玩家引用以便子类使用
	AActor* GetPlayerActor() const;
};