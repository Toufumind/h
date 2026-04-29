#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UICommandControllerComponent.generated.h"

UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UUICommandControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUICommandControllerComponent();

	UFUNCTION(BlueprintCallable, Category = "UI|Command")
	void ClaimReward(int32 RewardOptionIndex);

	UFUNCTION(BlueprintCallable, Category = "UI|Command")
	void SelectNode(FName NodeId);

	UFUNCTION(BlueprintCallable, Category = "UI|Command")
	void CompletePreparation(FName NodeId);
};
