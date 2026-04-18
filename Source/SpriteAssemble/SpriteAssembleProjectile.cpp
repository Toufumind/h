#include "SpriteAssembleProjectile.h"
#include "Components/SphereComponent.h"
#include "PaperFlipbookComponent.h" 
#include "PaperFlipbook.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpriteAssembleCharacter.h" 

ASpriteAssembleProjectile::ASpriteAssembleProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 【关键修复 1】：在物理层面上彻底忽略所有 WorldDynamic（也就是其他子弹）
	// 这样子弹之间就算重叠在一起，也完全当对方是空气，不会被物理引擎强行逼停！
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	CollisionComp->SetNotifyRigidBodyCollision(true);

	RootComponent = CollisionComp;

	ProjectileVisual = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ProjectileVisual"));
	ProjectileVisual->SetupAttachment(RootComponent);
	ProjectileVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// 【关键修复 2】：开启本地坐标系速度！
	// 只有开启这个，你生成的扇形角度（Pitch）才会生效，子弹才会散开飞，而不是全挤在一条线上。
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->Velocity = FVector(1.0f, 0.0f, 0.0f);

	InitialLifeSpan = 1.5f;
}


void ASpriteAssembleProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 绑定重叠事件（打中敌人）
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASpriteAssembleProjectile::OnProjectileOverlap);

	// 【新增】绑定命中事件（打中墙体）
	CollisionComp->OnComponentHit.AddDynamic(this, &ASpriteAssembleProjectile::OnProjectileHit);
}

void ASpriteAssembleProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsExploding || !OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	if (OtherActor && OtherActor->IsA<ASpriteAssembleProjectile>()) return;

	// 获取发射者
	AActor* Shooter = GetInstigator();

	// 判断：如果发射者是玩家，且打到了玩家，则忽略；如果发射者是怪物，且打到了怪物，则忽略。
	bool bShooterIsPlayer = Shooter && Shooter->IsA<ASpriteAssembleCharacter>();
	bool bHitPlayer = OtherActor->IsA<ASpriteAssembleCharacter>();

	if (bShooterIsPlayer && bHitPlayer) return; // 玩家不打玩家
	if (!bShooterIsPlayer && !bHitPlayer) return; // 怪物不打怪物

	float FinalDamage = DamageAmount;

	// 如果是玩家发射的，应用宝石加成
	if (bShooterIsPlayer)
	{
		ASpriteAssembleCharacter* Player = Cast<ASpriteAssembleCharacter>(Shooter);
		int32 DamageGems = Player->GetGemCount(EGemType::DamageUp);
		FinalDamage += (DamageGems * 10.0f);

		int32 LifestealGems = Player->GetGemCount(EGemType::Lifesteal);
		if (LifestealGems > 0)
		{
			Player->Heal(LifestealGems * 5.0f);
		}
	}

	UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	Explode();
}

// 【新增】撞墙直接爆炸
void ASpriteAssembleProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsExploding || !OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	// 【新增】：如果撞到的是另一个子弹，直接退出！
	if (OtherActor && OtherActor->IsA<ASpriteAssembleProjectile>()) return;

	Explode();
}

void ASpriteAssembleProjectile::LifeSpanExpired()
{
	if (!bIsExploding) Explode();
}

void ASpriteAssembleProjectile::Explode()
{
	bIsExploding = true;

	// 停止移动和碰撞
	ProjectileMovement->StopMovementImmediately();
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 播放爆炸动画
	if (ExplosionFlipbook)
	{
		ProjectileVisual->SetFlipbook(ExplosionFlipbook);
		ProjectileVisual->SetLooping(false);
		ProjectileVisual->PlayFromStart();

		// 定时销毁
		float AnimLength = ExplosionFlipbook->GetTotalDuration();
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ASpriteAssembleProjectile::OnExplosionFinished, AnimLength, false);
	}
	else
	{
		Destroy();
	}
}

void ASpriteAssembleProjectile::OnExplosionFinished()
{
	Destroy();
}