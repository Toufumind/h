#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplaySpecs.h"
#include "TargetingComponent.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FTargetingResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadOnly)
	float Distance = 0.0f;

	bool IsValid() const { return TargetActor != nullptr; }
};

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetingComponent();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Targeting")
	FTargetingResult FindNearestTarget(float Range, EGameplayTeam OwnerTeam) const;

private:
	bool IsValidTarget(AActor* Candidate, EGameplayTeam OwnerTeam) const;
};
