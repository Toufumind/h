#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GemTypes.h"
#include "GemInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGemInventoryChangedSignature, class UGemInventoryComponent*, GemInventory);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UGemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGemInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Gem")
	FGemInventoryChangedSignature OnGemInventoryChanged;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Gem")
	FGemRuntimeId AddGem(class UGemDefinition* Definition, int32 Level = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Gem")
	bool EquipGem(FGemRuntimeId RuntimeId, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Gem")
	bool ReorderEquippedGems(const TArray<FGemRuntimeId>& OrderedGemIds);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Gem")
	TArray<FGemRuntimeInstance> GetEquippedGemsInOrder() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Gem")
	const TArray<FGemRuntimeInstance>& GetOwnedGems() const { return OwnedGems; }

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|Gem")
	int32 SlotCount = 3;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Gem")
	TArray<FGemRuntimeInstance> OwnedGems;

	FGemRuntimeInstance* FindGem(FGemRuntimeId RuntimeId);
	const FGemRuntimeInstance* FindGem(FGemRuntimeId RuntimeId) const;
	void BroadcastChanged();
};
