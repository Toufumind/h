#include "SpriteAssembleEnemyBase.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "PaperFlipbookComponent.h"

ASpriteAssembleEnemyBase::ASpriteAssembleEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthTextComp->SetupAttachment(RootComponent);
	HealthTextComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HealthTextComp->SetHorizontalAlignment(EHTA_Center);

	PaperZDAnimComponent = CreateDefaultSubobject<UActorComponent>(TEXT("PaperZDAnimComponent"));

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);
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


FGameplayCommandResult ASpriteAssembleEnemyBase::ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec)
{
	if (bIsDead)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "EnemyAlreadyDead", "Enemy is already dead."));
	}

	TakeDamage(DamageSpec.FinalDamage, FDamageEvent(), nullptr, DamageSpec.SourceActor);
	return FGameplayCommandResult::Success();
}

bool ASpriteAssembleEnemyBase::IsTargetable_Implementation() const
{
	return !bIsDead;
}

EGameplayTeam ASpriteAssembleEnemyBase::GetGameplayTeam_Implementation() const
{
	return EGameplayTeam::Enemy;
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
