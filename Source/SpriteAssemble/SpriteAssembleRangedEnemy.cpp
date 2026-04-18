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
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction.X);
			SetActorRotation(Direction.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));
		}
		else if (Distance <= AttackRange)
		{
			bIsAttacking = true;
			// ÃæÏòÍæ¼Ò
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
		FVector SpawnLoc = GetActorLocation();
		FRotator SpawnRot = GetActorRotation();
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		GetWorld()->SpawnActor<ASpriteAssembleProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
	}
}