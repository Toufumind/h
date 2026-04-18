#pragma once
#include "CoreMinimal.h"
#include "SpriteAssembleEnemyBase.h"
#include "SpriteAssembleRangedEnemy.generated.h"

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleRangedEnemy : public ASpriteAssembleEnemyBase
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class ASpriteAssembleProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FireCooldown = 2.0f;

private:
	float LastFireTime = 0.0f;
	void FireProjectile();
};