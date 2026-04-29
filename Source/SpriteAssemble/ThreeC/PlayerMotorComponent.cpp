#include "PlayerMotorComponent.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

UPlayerMotorComponent::UPlayerMotorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMotorComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<ACharacter>(GetOwner());
	if (ACharacter* Character = GetCharacterOwner())
	{
		bWasFalling = Character->GetCharacterMovement()->IsFalling();
		if (!bWasFalling)
		{
			LastGroundedTime = GetWorldTime();
		}
	}
}

void UPlayerMotorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACharacter* Character = GetCharacterOwner();
	if (!Character || !Character->GetCharacterMovement())
	{
		return;
	}

	const bool bIsFallingNow = Character->GetCharacterMovement()->IsFalling();
	const bool bIsGroundedNow = Character->GetCharacterMovement()->IsMovingOnGround();
	if (bIsGroundedNow)
	{
		LastGroundedTime = GetWorldTime();
	}

	if (bWasFalling && !bIsFallingNow)
	{
		OnLandedFeedback.Broadcast();
	}
	bWasFalling = bIsFallingNow;

	TryConsumeBufferedJump();
}

void UPlayerMotorComponent::MoveHorizontal(float InputValue)
{
	ACharacter* Character = GetCharacterOwner();
	if (!Character || FMath::Abs(InputValue) <= HorizontalInputDeadZone)
	{
		return;
	}

	Character->AddMovementInput(FVector::ForwardVector, InputValue);
	if (AController* Controller = Character->GetController())
	{
		Controller->SetControlRotation(InputValue > 0.0f ? FRotator::ZeroRotator : FRotator(0.0f, 180.0f, 0.0f));
	}
}

void UPlayerMotorComponent::RequestJump()
{
	LastJumpPressedTime = GetWorldTime();
	if (CanJumpNow())
	{
		PerformJump();
	}
}

void UPlayerMotorComponent::StopJump()
{
	if (ACharacter* Character = GetCharacterOwner())
	{
		Character->StopJumping();
	}
}

bool UPlayerMotorComponent::IsJumpBuffered() const
{
	return HasBufferedJump();
}

bool UPlayerMotorComponent::IsUsingCoyoteTime() const
{
	ACharacter* Character = GetCharacterOwner();
	return Character && Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling() && HasRecentGround();
}

float UPlayerMotorComponent::GetHorizontalSpeed() const
{
	if (const ACharacter* Character = GetCharacterOwner())
	{
		return FVector::DotProduct(Character->GetVelocity(), FVector::ForwardVector);
	}

	return 0.0f;
}

bool UPlayerMotorComponent::IsFalling() const
{
	const ACharacter* Character = GetCharacterOwner();
	return Character && Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling();
}

ACharacter* UPlayerMotorComponent::GetCharacterOwner() const
{
	return CachedCharacter.IsValid() ? CachedCharacter.Get() : Cast<ACharacter>(GetOwner());
}

bool UPlayerMotorComponent::CanJumpNow() const
{
	ACharacter* Character = GetCharacterOwner();
	if (!Character || !Character->GetCharacterMovement())
	{
		return false;
	}

	return Character->GetCharacterMovement()->IsMovingOnGround() || HasRecentGround() || HasEdgeForgivenessGround();
}

bool UPlayerMotorComponent::HasBufferedJump() const
{
	return GetWorldTime() - LastJumpPressedTime <= JumpBufferSeconds;
}

bool UPlayerMotorComponent::HasRecentGround() const
{
	return GetWorldTime() - LastGroundedTime <= CoyoteTimeSeconds;
}

bool UPlayerMotorComponent::HasEdgeForgivenessGround() const
{
	if (!bEnableEdgeForgiveness)
	{
		return false;
	}

	const ACharacter* Character = GetCharacterOwner();
	const UCapsuleComponent* Capsule = Character ? Character->GetCapsuleComponent() : nullptr;
	const UWorld* World = GetWorld();
	if (!Character || !Capsule || !World)
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerMotorEdgeProbe), false, Character);
	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector Base = Character->GetActorLocation() - FVector(0.0f, 0.0f, CapsuleHalfHeight * 0.5f);

	for (const float Offset : { -EdgeProbeHorizontalOffset, EdgeProbeHorizontalOffset })
	{
		const FVector Start = Base + FVector(Offset, 0.0f, 0.0f);
		const FVector End = Start - FVector(0.0f, 0.0f, EdgeProbeDistance);
		FHitResult HitResult;
		if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, QueryParams))
		{
			return true;
		}
	}

	return false;
}

void UPlayerMotorComponent::TryConsumeBufferedJump()
{
	if (HasBufferedJump() && CanJumpNow())
	{
		PerformJump();
	}
}

void UPlayerMotorComponent::PerformJump()
{
	if (ACharacter* Character = GetCharacterOwner())
	{
		LastJumpPressedTime = -1000.0f;
		LastGroundedTime = -1000.0f;
		Character->Jump();
	}
}

float UPlayerMotorComponent::GetWorldTime() const
{
	return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}
