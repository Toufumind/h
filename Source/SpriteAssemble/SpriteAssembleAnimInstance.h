#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpriteAssembleAnimInstance.generated.h"

class ASpriteAssembleCharacter;

UCLASS()
class SPRITEASSEMBLE_API USpriteAssembleAnimInstance : public UObject
{
	GENERATED_BODY()

public:
	// PaperZD 的初始化回调 (必须带有 _Implementation)
	void OnInit();

	// PaperZD 的每帧更新回调 (名字是 OnTick，且必须带有 _Implementation)
	void OnTick(float DeltaTime);

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