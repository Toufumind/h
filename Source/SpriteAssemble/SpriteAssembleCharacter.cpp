// SpriteAssembleCharacter.cpp
#include "SpriteAssembleCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Attribute/AttributeComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/HealthComponent.h"
#include "Effect/EffectComponent.h"
#include "Gem/GemInventoryComponent.h"
#include "IceMirror/IceMirrorComponent.h"
#include "IceMirror/TargetingComponent.h"
#include "Presentation/HitFlashComponent.h"
#include "Presentation/PresentationComponent.h"
#include "SpellCircuit/SpellCircuitComponent.h"
#include "ThreeC/DamageReactionComponent.h"
#include "ThreeC/PlayerMotorComponent.h"
#include "UI/HealthViewModelComponent.h"
#include "UI/UICommandControllerComponent.h"
#include "Weapon/ShooterComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
void ApplyDebugPlaneMaterial(UStaticMeshComponent* Component, const FLinearColor& Color)
{
	if (!Component)
	{
		return;
	}

	UMaterialInterface* BaseMaterial = Component->GetMaterial(0);
	if (!BaseMaterial)
	{
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	UMaterialInstanceDynamic* DynamicMaterial = BaseMaterial
		? UMaterialInstanceDynamic::Create(BaseMaterial, Component)
		: nullptr;
	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
	Component->SetMaterial(0, DynamicMaterial);
}
}

ASpriteAssembleCharacter::ASpriteAssembleCharacter()
{
	// 禁用控制器的俯仰角(Pitch)和翻滚角(Roll)，只使用偏航角(Yaw)来翻转角色
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// 配置角色移动组件
	GetCharacterMovement()->bOrientRotationToMovement = false; // 2D中通常由Yaw控制朝向
	GetCharacterMovement()->GravityScale = 2.35f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 920.f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->MaxWalkSpeed = 620.0f;
	GetCharacterMovement()->MaxAcceleration = 5200.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 5200.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1600.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 80.0f;

	// 将角色的移动锁定在XZ平面上（防止角色向屏幕内外移动）
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	// 将Y轴锁定
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	// 创建摄像机摇臂
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 850.0f;
	CameraBoom->SocketOffset = FVector(120.0f, 0.0f, 120.0f);
	CameraBoom->bDoCollisionTest = false; // 横板游戏不需要摄像机碰撞测试
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 9.0f;
	CameraBoom->CameraLagMaxDistance = 180.0f;

	//让摄像机摇臂使用绝对世界旋转，防止它随着角色180度翻转！
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));

	// 创建侧视图摄像机
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 1650.0f;

	MarkerPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMarkerPlane"));
	MarkerPlaneComponent->SetupAttachment(RootComponent);
	MarkerPlaneComponent->SetRelativeLocation(FVector::ZeroVector);
	MarkerPlaneComponent->SetUsingAbsoluteRotation(true);
	MarkerPlaneComponent->SetWorldRotation(FRotator(0.0f, 0.0f, 90.0f));
	MarkerPlaneComponent->SetRelativeScale3D(FVector(0.55f, 1.1f, 1.0f));
	MarkerPlaneComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerPlaneComponent->SetHiddenInGame(false);
	MarkerPlaneComponent->SetCastShadow(false);
	MarkerPlaneBackComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMarkerPlaneBack"));
	MarkerPlaneBackComponent->SetupAttachment(RootComponent);
	MarkerPlaneBackComponent->SetRelativeLocation(FVector::ZeroVector);
	MarkerPlaneBackComponent->SetUsingAbsoluteRotation(true);
	MarkerPlaneBackComponent->SetWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	MarkerPlaneBackComponent->SetRelativeScale3D(FVector(0.55f, 1.1f, 1.0f));
	MarkerPlaneBackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerPlaneBackComponent->SetHiddenInGame(false);
	MarkerPlaneBackComponent->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MarkerPlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (MarkerPlaneMesh.Succeeded())
	{
		MarkerPlaneComponent->SetStaticMesh(MarkerPlaneMesh.Object);
		MarkerPlaneBackComponent->SetStaticMesh(MarkerPlaneMesh.Object);
	}

	// 如果你希望使用正交相机而不是透视相机，可以取消下面两行的注释：
	// SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	// SideViewCameraComponent->OrthoWidth = 2048.0f;

	// 初始化 PaperZD 动画组件
	PaperZDAnimComponent = CreateDefaultSubobject<UActorComponent>(TEXT("PaperZDAnimComponent"));

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	EffectComponent = CreateDefaultSubobject<UEffectComponent>(TEXT("EffectComponent"));
	GemInventoryComponent = CreateDefaultSubobject<UGemInventoryComponent>(TEXT("GemInventoryComponent"));
	IceMirrorComponent = CreateDefaultSubobject<UIceMirrorComponent>(TEXT("IceMirrorComponent"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	SpellCircuitComponent = CreateDefaultSubobject<USpellCircuitComponent>(TEXT("SpellCircuitComponent"));
	ShooterComponent = CreateDefaultSubobject<UShooterComponent>(TEXT("ShooterComponent"));
	PresentationComponent = CreateDefaultSubobject<UPresentationComponent>(TEXT("PresentationComponent"));
	HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("HitFlashComponent"));
	PlayerMotorComponent = CreateDefaultSubobject<UPlayerMotorComponent>(TEXT("PlayerMotorComponent"));
	DamageReactionComponent = CreateDefaultSubobject<UDamageReactionComponent>(TEXT("DamageReactionComponent"));
	HealthViewModelComponent = CreateDefaultSubobject<UHealthViewModelComponent>(TEXT("HealthViewModelComponent"));
	UICommandControllerComponent = CreateDefaultSubobject<UUICommandControllerComponent>(TEXT("UICommandControllerComponent"));

	EquippedGems.Init(EGemType::None, 5);
}

void ASpriteAssembleCharacter::BeginPlay()
{
	Super::BeginPlay();

	ApplyDebugPlaneMaterial(MarkerPlaneComponent, FLinearColor(0.08f, 0.42f, 1.0f, 0.92f));
	ApplyDebugPlaneMaterial(MarkerPlaneBackComponent, FLinearColor(0.08f, 0.42f, 1.0f, 0.92f));

	// 注册并添加强输入映射上下文
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	CurrentHealth = MaxHealth;
	if (HealthComponent)
	{
		HealthComponent->InitializeHealth(MaxHealth);
		CurrentHealth = HealthComponent->GetCurrentHealth();
		HealthComponent->OnDeath.AddDynamic(this, &ASpriteAssembleCharacter::HandleCharacterDeath);
	}
}

int32 ASpriteAssembleCharacter::GetGemCount(EGemType GemType) const
{
	int32 Count = 0;
	for (EGemType Gem : EquippedGems)
	{
		if (Gem == GemType) Count++;
	}
	return Count;
}

bool ASpriteAssembleCharacter::AddGem(EGemType NewGem)
{
	// 遍历 5 个插槽
	for (int32 i = 0; i < EquippedGems.Num(); ++i)
	{
		// 找到第一个空插槽
		if (EquippedGems[i] == EGemType::None)
		{
			EquippedGems[i] = NewGem; // 放入宝石
			return true;              // 拾取成功
		}
	}

	// 如果循环结束还没 return，说明 5 个格子都满了
	return false;
}

// 实现受伤与回血
float ASpriteAssembleCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FDamageSpec DamageSpec;
	DamageSpec.SourceActor = DamageCauser;
	DamageSpec.InstigatorActor = DamageCauser;
	DamageSpec.Team = EGameplayTeam::Enemy;
	DamageSpec.BaseDamage = DamageAmount;
	DamageSpec.FinalDamage = DamageAmount;
	ApplyDamageSpec(DamageSpec);
	return DamageAmount;
}

void ASpriteAssembleCharacter::Heal(float HealAmount)
{
	if (HealthComponent)
	{
		HealthComponent->Heal(HealAmount);
		CurrentHealth = HealthComponent->GetCurrentHealth();
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
}

FGameplayCommandResult ASpriteAssembleCharacter::ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec)
{
	if (!HealthComponent)
	{
		return FGameplayCommandResult::Unhandled();
	}
	if (DamageReactionComponent && !DamageReactionComponent->CanReceiveDamage())
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "DamageBlockedByReaction", "Damage was blocked by damage reaction state."));
	}

	FGameplayCommandResult Result = HealthComponent->ApplyDamageSpec(DamageSpec);
	CurrentHealth = HealthComponent->GetCurrentHealth();
	return Result;
}

bool ASpriteAssembleCharacter::IsTargetable_Implementation() const
{
	return HealthComponent ? !HealthComponent->IsDead() : CurrentHealth > 0.0f;
}

EGameplayTeam ASpriteAssembleCharacter::GetGameplayTeam_Implementation() const
{
	return EGameplayTeam::Player;
}

void ASpriteAssembleCharacter::ResetCharacterState()
{
	bIsShooting = false;
	bIsClimbing = false;

	GetCharacterMovement()->Activate(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


	if (HealthComponent)
	{
		HealthComponent->InitializeHealth(MaxHealth);
		CurrentHealth = HealthComponent->GetCurrentHealth();
	}
	if (DamageReactionComponent)
	{
		DamageReactionComponent->ResetReactionState();
	}
}

// 在 SetupPlayerInputComponent 函数中，补充射击和攀爬的绑定：
void ASpriteAssembleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 1. 绑定移动 (A/D键)
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpriteAssembleCharacter::Move);
		}

		// 2. 绑定跳跃 (空格键)
		if (JumpActionInput)
		{
			EnhancedInputComponent->BindAction(JumpActionInput, ETriggerEvent::Started, this, &ASpriteAssembleCharacter::JumpAction);
			EnhancedInputComponent->BindAction(JumpActionInput, ETriggerEvent::Completed, this, &ASpriteAssembleCharacter::StopJumpAction);
		}
		// 3. Climb (W/S key)
		// 3. 绑定射击 (J键/鼠标左键)
		if (ClimbActionInput)
		{
			EnhancedInputComponent->BindAction(ClimbActionInput, ETriggerEvent::Triggered, this, &ASpriteAssembleCharacter::ClimbAction);
		}
	}
}

// 【新增】攀爬逻辑实现
void ASpriteAssembleCharacter::ClimbAction(const FInputActionValue& Value)
{
	float ClimbValue = Value.Get<float>();

	// 只有在梯子范围内 (bCanClimb 为 true) 且有输入时才能攀爬
	if (bCanClimb && ClimbValue != 0.0f)
	{
		bIsClimbing = true;

		// 将移动模式改为飞行，消除重力影响，允许上下移动
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		// 沿 Z 轴（上下）移动
		AddMovementInput(FVector(0.0f, 0.0f, 1.0f), ClimbValue);
	}
}

// 【新增/重写】重写 Jump 函数，用于在跳跃时脱离梯子
void ASpriteAssembleCharacter::Jump()
{
	if (bIsClimbing)
	{
		// 如果正在攀爬，按下跳跃键则脱离梯子，恢复重力
		bIsClimbing = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	Super::Jump(); // 调用原生的跳跃逻辑
}

void ASpriteAssembleCharacter::Move(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();

	if (Controller != nullptr && MovementValue != 0.0f)
	{
		PlayerMotorComponent ? PlayerMotorComponent->MoveHorizontal(MovementValue) : AddMovementInput(FVector(1.0f, 0.0f, 0.0f), MovementValue);
	}
}

void ASpriteAssembleCharacter::JumpAction(const FInputActionValue& Value)
{
	PlayerMotorComponent ? PlayerMotorComponent->RequestJump() : Jump();
}

void ASpriteAssembleCharacter::StopJumpAction(const FInputActionValue& Value)
{
	PlayerMotorComponent ? PlayerMotorComponent->StopJump() : StopJumping();
}

void ASpriteAssembleCharacter::HandleCharacterDeath(UHealthComponent* InHealthComponent)
{
	bIsShooting = false;
	bIsClimbing = false;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Deactivate();

}
