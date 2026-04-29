// SpriteAssembleCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "Core/GameplayInterfaces.h"
#include "SpriteAssembleCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UInputAction;
class UActorComponent;
class UAttributeComponent;
class UCombatComponent;
class UEffectComponent;
class UGemInventoryComponent;
class UHealthComponent;
class UHealthViewModelComponent;
class UHitFlashComponent;
class UIceMirrorComponent;
class UDamageReactionComponent;
class UPlayerMotorComponent;
class UPresentationComponent;
class UShooterComponent;
class USpellCircuitComponent;
class UTargetingComponent;
class UUICommandControllerComponent;
// TODO: remove EGemType — superseded by UGemInventoryComponent + GemCommandWorldSubsystem, kept for BP compatibility
UENUM(BlueprintType)
enum class EGemType : uint8
{
	None UMETA(DisplayName = "Empty"),
	Split UMETA(DisplayName = "Split Projectile"),
	DamageUp UMETA(DisplayName = "Damage Up"),
	Lifesteal UMETA(DisplayName = "Lifesteal")
};

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleCharacter : public APaperCharacter, public IDamageable, public ITargetable, public ITeamAgent
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

	virtual FGameplayCommandResult ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec) override;
	virtual bool IsTargetable_Implementation() const override;
	virtual EGameplayTeam GetGameplayTeam_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "3C|Character")
	void ResetCharacterState();

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

	void ClimbAction(const FInputActionValue& Value);

	// 修改原有的跳跃函数，用于打断攀爬
	

private:
	// 摄像机摇臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 侧视图像机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Editor", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MarkerPlaneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Editor", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MarkerPlaneBackComponent;

	// TODO: remove PaperZDAnimComponent — superseded by PresentationComponent, kept for BP compatibility
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UActorComponent* PaperZDAnimComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UEffectComponent* EffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UGemInventoryComponent* GemInventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UIceMirrorComponent* IceMirrorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UTargetingComponent* TargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	USpellCircuitComponent* SpellCircuitComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UShooterComponent* ShooterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (AllowPrivateAccess = "true"))
	UPresentationComponent* PresentationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (AllowPrivateAccess = "true"))
	UHitFlashComponent* HitFlashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "3C", meta = (AllowPrivateAccess = "true"))
	UPlayerMotorComponent* PlayerMotorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "3C", meta = (AllowPrivateAccess = "true"))
	UDamageReactionComponent* DamageReactionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHealthViewModelComponent* HealthViewModelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UUICommandControllerComponent* UICommandControllerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbActionInput;

	// 【新增】角色身后的精灵组件 (无碰撞)
	// TODO: review — SpiritComponent was removed; wire replacement if needed

	UFUNCTION()
	void HandleCharacterDeath(UHealthComponent* InHealthComponent);
};
