#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMotorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMotorFeedbackSignature);

UCLASS(ClassGroup=(ThreeC), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UPlayerMotorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerMotorComponent();

	UPROPERTY(BlueprintAssignable, Category = "3C|Motor")
	FMotorFeedbackSignature OnLandedFeedback;

	UFUNCTION(BlueprintCallable, Category = "3C|Motor")
	void MoveHorizontal(float InputValue);

	UFUNCTION(BlueprintCallable, Category = "3C|Motor")
	void RequestJump();

	UFUNCTION(BlueprintCallable, Category = "3C|Motor")
	void StopJump();

	UFUNCTION(BlueprintPure, Category = "3C|Motor")
	bool IsJumpBuffered() const;

	UFUNCTION(BlueprintPure, Category = "3C|Motor")
	bool IsUsingCoyoteTime() const;

	UFUNCTION(BlueprintPure, Category = "3C|Motor")
	float GetHorizontalSpeed() const;

	UFUNCTION(BlueprintPure, Category = "3C|Motor")
	bool IsFalling() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	float HorizontalInputDeadZone = 0.1f;

	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	float CoyoteTimeSeconds = 0.12f;

	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	float JumpBufferSeconds = 0.12f;

	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	float EdgeProbeDistance = 80.0f;

	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	float EdgeProbeHorizontalOffset = 34.0f;

	UPROPERTY(EditAnywhere, Category = "3C|Motor")
	bool bEnableEdgeForgiveness = true;

	UPROPERTY(VisibleAnywhere, Category = "3C|Motor")
	bool bWasFalling = false;

	float LastGroundedTime = -1000.0f;
	float LastJumpPressedTime = -1000.0f;

	TWeakObjectPtr<class ACharacter> CachedCharacter;

	ACharacter* GetCharacterOwner() const;
	bool CanJumpNow() const;
	bool HasBufferedJump() const;
	bool HasRecentGround() const;
	bool HasEdgeForgivenessGround() const;
	void TryConsumeBufferedJump();
	void PerformJump();
	float GetWorldTime() const;
};
