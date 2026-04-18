#pragma once
#include "CoreMinimal.h"
#include "SpriteAssembleEnemyBase.h"
#include "SpriteAssembleMeleeEnemy.generated.h"

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleMeleeEnemy : public ASpriteAssembleEnemyBase
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 100.0f;
};