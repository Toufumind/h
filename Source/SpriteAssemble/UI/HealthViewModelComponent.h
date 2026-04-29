#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ViewModelTypes.h"
#include "HealthViewModelComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthViewSnapshotChangedSignature, const FHealthViewSnapshot&, Snapshot);

UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UHealthViewModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthViewModelComponent();

	UPROPERTY(BlueprintAssignable, Category = "UI|ViewModel")
	FHealthViewSnapshotChangedSignature OnSnapshotChanged;

	UFUNCTION(BlueprintPure, Category = "UI|ViewModel")
	FHealthViewSnapshot GetSnapshot() const { return Snapshot; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "UI|ViewModel")
	FHealthViewSnapshot Snapshot;

	UPROPERTY()
	TObjectPtr<UHealthComponent> HealthComponent = nullptr;

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* InHealthComponent, float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleDeath(UHealthComponent* InHealthComponent);

	void RefreshSnapshot();
};
