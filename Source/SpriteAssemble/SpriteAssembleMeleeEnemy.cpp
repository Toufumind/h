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
			// ×·»÷Íæ¼Ò
			bIsAttacking = false;
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction.X);
			SetActorRotation(Direction.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));
		}
		else if (Distance <= AttackRange)
		{
			// ¹¥»÷·¶Î§
			bIsAttacking = true;
		}
		else
		{
			// ³¬³ö·¶Î§£¬´ý»ú
			bIsAttacking = false;
		}
	}
}