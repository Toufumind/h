// SpriteAssembleCharacter.cpp
#include "SpriteAssembleCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperZDAnimationComponent.h" // PaperZD 头文件
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "PaperFlipbookComponent.h"
#include "SpriteAssembleProjectile.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpriteAssembleEnemyBase.h"

ASpriteAssembleCharacter::ASpriteAssembleCharacter()
{
	// 禁用控制器的俯仰角(Pitch)和翻滚角(Roll)，只使用偏航角(Yaw)来翻转角色
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 配置角色移动组件
	GetCharacterMovement()->bOrientRotationToMovement = false; // 2D中通常由Yaw控制朝向
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	// 将角色的移动锁定在XZ平面上（防止角色向屏幕内外移动）
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	// 将Y轴锁定
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	// 创建摄像机摇臂
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->bDoCollisionTest = false; // 横板游戏不需要摄像机碰撞测试
	CameraBoom->bUsePawnControlRotation = false;

	//让摄像机摇臂使用绝对世界旋转，防止它随着角色180度翻转！
	CameraBoom->SetUsingAbsoluteRotation(true);

	// 创建侧视图摄像机
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false;

	// 如果你希望使用正交相机而不是透视相机，可以取消下面两行的注释：
	// SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	// SideViewCameraComponent->OrthoWidth = 2048.0f;

	// 初始化 PaperZD 动画组件
	PaperZDAnimComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("PaperZDAnimComponent"));

	// 创建精灵组件并附加到角色身上
	SpiritComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpiritComponent"));
	SpiritComponent->SetupAttachment(RootComponent);
	// 将精灵位置设置在角色身后(-60)和稍微靠上(40)的位置
	SpiritComponent->SetRelativeLocation(FVector(-60.0f, 0.0f, 40.0f));
	SpiritComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 精灵不需要碰撞
	SpiritComponent->SetHiddenInGame(true); // 默认隐藏，开火时才显示

	// 【新增这一行】：让精灵的动画只播放一次，不要无限循环
	SpiritComponent->SetLooping(false);

	// 【新增这两行】：让精灵使用绝对旋转，不跟着角色一起转！
	SpiritComponent->SetUsingAbsoluteRotation(true);
	SpiritComponent->SetWorldRotation(FRotator::ZeroRotator); // 初始强制朝右

	// 添加血量显示组件
	HealthTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthTextComp->SetupAttachment(RootComponent);
	HealthTextComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // 调整高度到头顶
	HealthTextComp->SetHorizontalAlignment(EHTA_Center);
	HealthTextComp->SetTextRenderColor(FColor::Green);

	EquippedGems.Init(EGemType::None, 5);
}

void ASpriteAssembleCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	UpdateHealthUI();

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
}

void ASpriteAssembleCharacter::UpdateHealthUI()
{
	if (HealthTextComp)
	{
		HealthTextComp->SetText(FText::AsNumber(FMath::Max(0.0f, CurrentHealth)));
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

EGemType ASpriteAssembleCharacter::SwapGem(int32 SlotIndex, EGemType NewGem)
{
	if (EquippedGems.IsValidIndex(SlotIndex))
	{
		EGemType OldGem = EquippedGems[SlotIndex]; // 记录旧宝石
		EquippedGems[SlotIndex] = NewGem;          // 装上新宝石
		return OldGem;                             // 返回旧的用来生成掉落物
	}
	return EGemType::None;
}

// 实现受伤与回血
float ASpriteAssembleCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth <= 0.0f) return 0.0f; // 已经死了

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	UpdateHealthUI();

	if (CurrentHealth <= 0.0f)
	{
		GameOver();
	}
	return DamageAmount;
}

void ASpriteAssembleCharacter::Heal(float HealAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	UpdateHealthUI();
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

		// 3. 绑定射击 (J键/鼠标左键)
		if (ShootActionInput)
		{
			EnhancedInputComponent->BindAction(ShootActionInput, ETriggerEvent::Started, this, &ASpriteAssembleCharacter::ShootPressed);
			EnhancedInputComponent->BindAction(ShootActionInput, ETriggerEvent::Completed, this, &ASpriteAssembleCharacter::ShootReleased);
		}

		// 4. 绑定攀爬 (W/S键)
		if (ClimbActionInput)
		{
			EnhancedInputComponent->BindAction(ClimbActionInput, ETriggerEvent::Triggered, this, &ASpriteAssembleCharacter::ClimbAction);
		}
	}
}

// 【新增】射击逻辑实现
// 找到 ShootPressed 函数中计算 SpawnLocation 的地方并修改：
void ASpriteAssembleCharacter::ShootPressed(const FInputActionValue& Value)
{
	if (bIsSpiritAttacking) return;
	bIsShooting = true;
	bIsSpiritAttacking = true;
	bool bFacingRight = (GetActorForwardVector().X > 0.0f);

	if (SpiritComponent)
	{
		SpiritComponent->SetRelativeLocation(FVector(-60.0f, -10.0f, 40.0f));
		SpiritComponent->SetWorldRotation(bFacingRight ? FRotator(0.0f, 0.0f, 0.0f) : FRotator(0.0f, 180.0f, 0.0f));

		// 根据当前攻击模式切换虚影动画
		UPaperFlipbook* TargetFlipbook = (CurrentAttackMode == EAttackMode::Ranged) ? RangedSpiritFlipbook : MeleeSpiritFlipbook;
		if (TargetFlipbook)
		{
			SpiritComponent->SetFlipbook(TargetFlipbook);
		}

		SpiritComponent->SetHiddenInGame(false);
		SpiritComponent->PlayFromStart();
	}

	// 统一获取发射/攻击基础位置
	FVector BaseLocation = SpiritComponent ? SpiritComponent->GetComponentLocation() : GetActorLocation();
	FVector SpawnLocation = BaseLocation + FVector(bFacingRight ? ProjectileSpawnOffset.X : -ProjectileSpawnOffset.X, ProjectileSpawnOffset.Y, ProjectileSpawnOffset.Z);
	// 【修复任务1】强制让Y轴与角色保持一致
	SpawnLocation.Y = GetActorLocation().Y;

	if (CurrentAttackMode == EAttackMode::Ranged)
	{
		// === 远程攻击逻辑 ===
		if (ProjectileClass)
		{
			FRotator BaseRotation = bFacingRight ? FRotator(0.0f, 0.0f, 0.0f) : FRotator(0.0f, 180.0f, 0.0f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			int32 SplitCount = GetGemCount(EGemType::Split);
			int32 TotalProjectiles = 1 + (SplitCount * 2);
			float AngleStep = 5.0f;

			for (int32 i = 0; i < TotalProjectiles; i++)
			{
				float AngleOffset = (i == 0) ? 0.0f : ((i % 2 == 1) ? 1.0f : -1.0f) * FMath::CeilToFloat(i / 2.0f) * AngleStep;
				FRotator SpawnRotation = BaseRotation;
				SpawnRotation.Pitch += AngleOffset;
				GetWorld()->SpawnActor<ASpriteAssembleProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
			}
		}
	}
	else if (CurrentAttackMode == EAttackMode::Melee)
	{
		// === 近战攻击逻辑 ===
		// 【任务2】计算宝石加成
		int32 SplitGems = GetGemCount(EGemType::Split);
		int32 DamageGems = GetGemCount(EGemType::DamageUp);
		int32 LifestealGems = GetGemCount(EGemType::Lifesteal);

		float FinalRange = MeleeBaseRange + (SplitGems * 50.0f); // 分裂宝石增加范围
		float FinalDamage = MeleeBaseDamage + (DamageGems * 10.0f); // 伤害宝石增加伤害

		// 进行球体检测 (Sphere Trace)
		FVector TraceStart = SpawnLocation;
		FVector TraceEnd = TraceStart + (bFacingRight ? FVector(FinalRange, 0, 0) : FVector(-FinalRange, 0, 0));
		TArray<FHitResult> HitResults;
		FCollisionShape SphereShape = FCollisionShape::MakeSphere(40.0f); // 攻击判定半径

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn, SphereShape, QueryParams);
		if (bHit)
		{
			for (const FHitResult& Hit : HitResults)
			{
				if (Hit.GetActor() && Hit.GetActor()->IsA<ASpriteAssembleEnemyBase>())
				{
					UGameplayStatics::ApplyDamage(Hit.GetActor(), FinalDamage, GetController(), this, UDamageType::StaticClass());

					// 吸血效果
					if (LifestealGems > 0)
					{
						Heal(LifestealGems * 5.0f);
					}
				}
			}
		}
	}

	CurrentAttackDuration = 0.5f;
	if (SpiritComponent && SpiritComponent->GetFlipbookLength() > 0.0f)
	{
		CurrentAttackDuration = SpiritComponent->GetFlipbookLength();
	}
	GetWorld()->GetTimerManager().SetTimer(SpiritAttackTimerHandle, this, &ASpriteAssembleCharacter::OnSpiritAttackFinished, CurrentAttackDuration, false);
}

float ASpriteAssembleCharacter::GetAttackCooldownPercent() const
{
	// 如果没有在攻击，或者时长异常，说明冷却已完毕，返回 1.0 (100%)
	if (!bIsSpiritAttacking || CurrentAttackDuration <= 0.0f)
	{
		return 1.0f;
	}

	// 获取定时器已经流逝的时间
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(SpiritAttackTimerHandle);

	// 计算百分比并限制在 0~1 之间
	return FMath::Clamp(ElapsedTime / CurrentAttackDuration, 0.0f, 1.0f);
}

void ASpriteAssembleCharacter::SetAttackMode(EAttackMode NewMode)
{
	CurrentAttackMode = NewMode;
}

void ASpriteAssembleCharacter::ShootReleased(const FInputActionValue& Value)
{
	// 松开鼠标时，只恢复角色的状态，【不再】隐藏精灵
	bIsShooting = false;
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
		// 角色水平移动
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), MovementValue);

		// 判断移动方向并翻转角色
		if (MovementValue > 0.0f)
		{
			// 向右
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			// 把原先这里的 SpiritComponent 设置全删掉！
		}
		else if (MovementValue < 0.0f)
		{
			// 向左
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
			// 把原先这里的 SpiritComponent 设置全删掉！
		}
	}
}

void ASpriteAssembleCharacter::JumpAction(const FInputActionValue& Value)
{
	Jump(); // 调用 ACharacter 原生的跳跃逻辑
}

void ASpriteAssembleCharacter::StopJumpAction(const FInputActionValue& Value)
{
	StopJumping();
}

// 【新增】当精灵动画播放完毕时自动执行
void ASpriteAssembleCharacter::OnSpiritAttackFinished()
{
	// 解除攻击锁，允许下一次射击
	bIsSpiritAttacking = false;

	// 隐藏精灵
	if (SpiritComponent)
	{
		SpiritComponent->SetHiddenInGame(true);
	}
}

void ASpriteAssembleCharacter::GameOver()
{
	// 禁用输入
	DisableInput(Cast<APlayerController>(GetController()));

	// 重启当前关卡（也可以换成显示UI等逻辑）
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}