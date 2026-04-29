#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayEvent.h"
#include "ShotImpactPresenterWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShotImpactPresentedSignature, const FShotHitSpec&, HitSpec);

UCLASS()
class SPRITEASSEMBLE_API UShotImpactPresenterWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category = "Presentation")
	FShotImpactPresentedSignature OnShotImpactPresented;

private:
	FDelegateHandle ShotHitHandle;

	void HandleShotHit(const FGameplayEvent& Event);
};
