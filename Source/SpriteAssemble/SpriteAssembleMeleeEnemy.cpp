#include "SpriteAssembleMeleeEnemy.h"

void ASpriteAssembleMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDead) return;

	AActor* Player = GetPlayerActor();
	if (Player)
	{
		float Distance = FVector::Distance(GetActorLocation(), Player->GetActorLocation());

		if (Distance <= DetectionRadius && Distance > AttackRange)
		{
			// 追击
			bIsAttacking = false;
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// 使用基类的移动方法（自动处理飞行和地面）
			MoveTowardsTarget(Direction);

			SetActorRotation(Direction.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));
		}
		else if (Distance <= AttackRange)
		{
			// 攻击范围内
			bIsAttacking = true;
		}
		else
		{
			// 范围外
			bIsAttacking = false;
		}
	}
}