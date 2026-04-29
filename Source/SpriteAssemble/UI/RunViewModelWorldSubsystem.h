#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ViewModelTypes.h"
#include "RunViewModelWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRunViewSnapshotChangedSignature, const FRunViewSnapshot&, Snapshot);

UCLASS()
class SPRITEASSEMBLE_API URunViewModelWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UI|ViewModel")
	FRunViewSnapshotChangedSignature OnSnapshotChanged;

	UFUNCTION(BlueprintPure, Category = "UI|ViewModel")
	FRunViewSnapshot GetSnapshot() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void Refresh();
};
