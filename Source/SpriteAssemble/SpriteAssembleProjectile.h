#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteAssembleProjectile.generated.h"

class USphereComponent;
class UPaperFlipbookComponent;
class UProjectileMovementComponent;
class UPaperFlipbook;

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASpriteAssembleProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void LifeSpanExpired() override; // 重写销毁逻辑以播放爆炸

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperFlipbookComponent* ProjectileVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 爆炸时的动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* ExplosionFlipbook;

	// 子弹伤害值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageAmount = 20.0f;

	// 碰撞处理函数
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 【新增】命中处理函数 (处理打到墙体)
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	bool bIsExploding = false;
	void Explode();
	void OnExplosionFinished();
	FTimerHandle ExplosionTimerHandle;
};