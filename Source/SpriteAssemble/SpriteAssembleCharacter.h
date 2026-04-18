// SpriteAssembleCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "SpriteAssembleCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPaperZDAnimationComponent;
UENUM(BlueprintType)
enum class EGemType : uint8
{
	None UMETA(DisplayName = "Empty"),
	Split UMETA(DisplayName = "Split Projectile"),
	DamageUp UMETA(DisplayName = "Damage Up"),
	Lifesteal UMETA(DisplayName = "Lifesteal")
};

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	// --- 增强输入系统资源 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpActionInput;

	// 【新增】动作状态变量
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsShooting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsClimbing;

	// 【新增】是否在梯子/攀爬物范围内的标记 (后续可以通过碰撞盒触发)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	bool bCanClimb;

	// --- 血量系统 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	// 受伤与回血
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float HealAmount);

	// --- 宝石插槽系统 ---
	// 5个插槽的数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gems")
	TArray<EGemType> EquippedGems;

	// 获取装备了几个同种宝石
	UFUNCTION(BlueprintPure, Category = "Gems")
	int32 GetGemCount(EGemType GemType) const;

	// 尝试添加一个宝石。如果5个插槽满了返回false，成功添加返回true
	UFUNCTION(BlueprintCallable, Category = "Gems")
	bool AddGem(EGemType NewGem);

	ASpriteAssembleCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	// 输入绑定的回调函数
	void Move(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);
	void StopJumpAction(const FInputActionValue& Value);

	void ShootPressed(const FInputActionValue& Value);
	void ShootReleased(const FInputActionValue& Value);
	void ClimbAction(const FInputActionValue& Value);

	// 修改原有的跳跃函数，用于打断攀爬
	

private:
	// 摄像机摇臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 侧视图像机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* SideViewCameraComponent;

	// PaperZD 动画组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UPaperZDAnimationComponent* PaperZDAnimComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootActionInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbActionInput;

	// 【新增】角色身后的精灵组件 (无碰撞)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spirit", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* SpiritComponent;

	// 【新增】用于在蓝图中指定要发射的子弹类
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spirit", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASpriteAssembleProjectile> ProjectileClass;

	// 【新增】子弹生成的相对偏移量（相对于精灵中心）。可以在蓝图细节面板中随时修改！
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spirit", meta = (AllowPrivateAccess = "true"))
	FVector ProjectileSpawnOffset = FVector(0.0f, 0.0f, 0.0f);

	// 【新增】防止疯狂连点导致鬼畜的“攻击锁”
	bool bIsSpiritAttacking = false;

	// 【新增】定时器句柄，用来控制精灵消失的时间
	FTimerHandle SpiritAttackTimerHandle;

	// 【新增】动画播放完毕后调用的回调函数
	void OnSpiritAttackFinished();
};