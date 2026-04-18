#include "SpriteAssembleRangedEnemy.h"
#include "SpriteAssembleProjectile.h"

void ASpriteAssembleRangedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDead) return;

	AActor* Player = GetPlayerActor();
	if (Player)
	{
		float Distance = FVector::Distance(GetActorLocation(), Player->GetActorLocation());

		if (Distance <= DetectionRadius && Distance > AttackRange)
		{
			bIsAttacking = false;
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// 使用基类的移动方法（自动处理飞行和地面）
			MoveTowardsTarget(Direction);

			SetActorRotation(Direction.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));
		}
		else if (Distance <= AttackRange)
		{
			bIsAttacking = true;
			// 转向
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			SetActorRotation(Direction.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));

			if (GetWorld()->GetTimeSeconds() - LastFireTime > FireCooldown)
			{
				FireProjectile();
				LastFireTime = GetWorld()->GetTimeSeconds();
			}
		}
		else
		{
			bIsAttacking = false;
		}
	}
}

void ASpriteAssembleRangedEnemy::FireProjectile()
{
	if (ProjectileClass)
	{
		FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 20.0f); // 稍微抬高一点，防止和地面碰撞
		FRotator SpawnRot = GetActorRotation();
		FActorSpawnParameters Params;
		Params.Owner = this; // 重要：让子弹知道它的主人是怪物
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 强制生成

		GetWorld()->SpawnActor<ASpriteAssembleProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
	}
}