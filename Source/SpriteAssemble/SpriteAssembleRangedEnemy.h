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

	// TODO: review — wire new combat system for ranged attack
private:
};
