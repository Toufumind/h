#include "SpriteAssembleEnemyBase.h"
#include "Components/TextRenderComponent.h"
#include "PaperZDAnimationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"

ASpriteAssembleEnemyBase::ASpriteAssembleEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthTextComp->SetupAttachment(RootComponent);
	HealthTextComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HealthTextComp->SetHorizontalAlignment(EHTA_Center);

	PaperZDAnimComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("PaperZDAnimComponent"));

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	// 让怪物的胶囊体与玩家(Pawn)发生重叠(Overlap)而不是阻挡(Block)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASpriteAssembleEnemyBase::OnEnemyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsDead && OtherActor && OtherActor == GetPlayerActor())
	{
		// 对玩家造成 15 点伤害（数值可改）
		UGameplayStatics::ApplyDamage(OtherActor, 15.0f, GetController(), this, UDamageType::StaticClass());
	}
}

float ASpriteAssembleEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	CurrentHealth -= DamageAmount;
	UpdateHealthUI();

	// 【新增】：受击闪白效果
	// 将 Sprite 颜色设置为超高亮度的白色（HDR发光效果，或者可以换成红色 FLinearColor(10,0,0,1)）
	// 改成红色，且强度设高一点
	GetSprite()->SetSpriteColor(FLinearColor(10.0f, 0.0f, 0.0f, 1.0f));
	// 0.1秒后恢复正常颜色
	GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, &ASpriteAssembleEnemyBase::ResetFlashColor, 0.1f, false);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	return DamageAmount;
}


void ASpriteAssembleEnemyBase::ResetFlashColor()
{
	if (GetSprite())
	{
		GetSprite()->SetSpriteColor(FLinearColor::White); // 恢复原色
	}
}

void ASpriteAssembleEnemyBase::BeginPlay()
{
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASpriteAssembleEnemyBase::OnEnemyOverlap);
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	UpdateHealthUI();

	// 如果勾选了飞行，关闭重力并切换移动模式
	if (bIsFlying)
	{
		GetCharacterMovement()->GravityScale = 0.0f;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

// 实现通用移动逻辑
void ASpriteAssembleEnemyBase::MoveTowardsTarget(FVector Direction)
{
	// 水平移动
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction.X);

	// 如果是飞行怪物，增加垂直移动
	if (bIsFlying)
	{
		AddMovementInput(FVector(0.0f, 0.0f, 1.0f), Direction.Z);
	}
}

void ASpriteAssembleEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsDead)
	{
		bIsMoving = GetVelocity().Length() > 10.0f;
	}
}



void ASpriteAssembleEnemyBase::UpdateHealthUI()
{
	if (HealthTextComp)
	{
		HealthTextComp->SetText(FText::AsNumber(FMath::Max(0.0f, CurrentHealth)));
	}
}

void ASpriteAssembleEnemyBase::Die()
{
	bIsDead = true;
	bIsMoving = false;
	bIsAttacking = false;
	GetCharacterMovement()->DisableMovement();
	HealthTextComp->SetVisibility(false);

	// 关闭碰撞防止诈尸阻挡玩家
	SetActorEnableCollision(false);

	// 真正的销毁可以在PaperZD的动画通知（AnimNotify）中调用，或者在这里设置一个定时器销毁
	SetLifeSpan(3.0f); // 3秒后彻底销毁Actor
}

AActor* ASpriteAssembleEnemyBase::GetPlayerActor() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}