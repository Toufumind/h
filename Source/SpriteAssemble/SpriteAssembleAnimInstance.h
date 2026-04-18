#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "SpriteAssembleAnimInstance.generated.h"

class ASpriteAssembleCharacter;

UCLASS()
class SPRITEASSEMBLE_API USpriteAssembleAnimInstance : public UPaperZDAnimInstance
{
	GENERATED_BODY()

public:
	// PaperZD 的初始化回调 (必须带有 _Implementation)
	virtual void OnInit_Implementation() override;

	// PaperZD 的每帧更新回调 (名字是 OnTick，且必须带有 _Implementation)
	virtual void OnTick_Implementation(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ASpriteAssembleCharacter* OwningCharacter;

	// --- 供蓝图状态机使用的变量 ---
	UPROPERTY(BlueprintReadOnly, Category = "Animation Data")
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Data")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Data")
	bool bIsMoving;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Data")
	bool bIsShooting;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Data")
	bool bIsClimbing;
};