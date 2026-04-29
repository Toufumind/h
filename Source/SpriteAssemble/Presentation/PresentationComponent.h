#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayInterfaces.h"
#include "PresentationComponent.generated.h"

UCLASS(ClassGroup=(Presentation), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UPresentationComponent : public UActorComponent, public IPresentationStateProvider
{
	GENERATED_BODY()

public:
	UPresentationComponent();

	UFUNCTION(BlueprintCallable, Category = "Presentation")
	void SetFacingDirection(FVector InFacingDirection);

	UFUNCTION(BlueprintCallable, Category = "Presentation")
	void AddStateTag(FGameplayTag StateTag);

	UFUNCTION(BlueprintCallable, Category = "Presentation")
	void RemoveStateTag(FGameplayTag StateTag);

	virtual FPresentationState GetPresentationState_Implementation() const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Presentation")
	FPresentationState PresentationState;
};
