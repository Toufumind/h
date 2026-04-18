#include "SpriteAssembleAnimInstance.h"
#include "SpriteAssembleCharacter.h" // 引入你的角色类
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

void USpriteAssembleAnimInstance::OnInit_Implementation()
{
	// 注意这里要调用父类的 _Implementation
	Super::OnInit_Implementation();

	// 在初始化时获取并缓存角色指针
	//OwningCharacter = Cast<ASpriteAssembleCharacter>(GetOwningActor());
}

void USpriteAssembleAnimInstance::OnTick_Implementation(float DeltaTime)
{
	// 注意这里要调用父类的 _Implementation
	Super::OnTick_Implementation(DeltaTime);

	if (!OwningCharacter)
	{
		OwningCharacter = Cast<ASpriteAssembleCharacter>(GetOwningActor());
	}

	// 每帧更新动画所需的变量
	if (OwningCharacter)
	{
		Speed = OwningCharacter->GetVelocity().Length();

		bIsMoving = Speed > 10.0f;
		bIsFalling = OwningCharacter->GetCharacterMovement()->IsFalling();

		// 同步角色的状态变量 (上一步在 Character 里加的)
		bIsShooting = OwningCharacter->bIsShooting;
		bIsClimbing = OwningCharacter->bIsClimbing;
	}
}