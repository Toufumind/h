#include "GemInventoryComponent.h"
#include "GemDefinition.h"

UGemInventoryComponent::UGemInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FGemRuntimeId UGemInventoryComponent::AddGem(UGemDefinition* Definition, int32 Level)
{
	if (!Definition)
	{
		return FGemRuntimeId();
	}

	FGemRuntimeInstance Instance;
	Instance.RuntimeId = FGemRuntimeId::NewId();
	Instance.Definition = Definition;
	Instance.Level = FMath::Clamp(Level, 1, FMath::Max(1, Definition->MaxLevel));
	OwnedGems.Add(Instance);

	BroadcastChanged();
	return Instance.RuntimeId;
}

bool UGemInventoryComponent::EquipGem(FGemRuntimeId RuntimeId, int32 SlotIndex)
{
	if (!RuntimeId.IsValid() || SlotIndex < 0 || SlotIndex >= SlotCount)
	{
		return false;
	}

	FGemRuntimeInstance* TargetGem = FindGem(RuntimeId);
	if (!TargetGem)
	{
		return false;
	}

	for (FGemRuntimeInstance& Gem : OwnedGems)
	{
		if (Gem.bEquipped && Gem.EquippedSlotIndex == SlotIndex)
		{
			Gem.bEquipped = false;
			Gem.EquippedSlotIndex = INDEX_NONE;
		}
	}

	TargetGem->bEquipped = true;
	TargetGem->EquippedSlotIndex = SlotIndex;
	BroadcastChanged();
	return true;
}

bool UGemInventoryComponent::ReorderEquippedGems(const TArray<FGemRuntimeId>& OrderedGemIds)
{
	if (OrderedGemIds.Num() > SlotCount)
	{
		return false;
	}

	for (FGemRuntimeInstance& Gem : OwnedGems)
	{
		Gem.bEquipped = false;
		Gem.EquippedSlotIndex = INDEX_NONE;
	}

	for (int32 Index = 0; Index < OrderedGemIds.Num(); ++Index)
	{
		FGemRuntimeInstance* Gem = FindGem(OrderedGemIds[Index]);
		if (!Gem)
		{
			return false;
		}

		Gem->bEquipped = true;
		Gem->EquippedSlotIndex = Index;
	}

	BroadcastChanged();
	return true;
}

TArray<FGemRuntimeInstance> UGemInventoryComponent::GetEquippedGemsInOrder() const
{
	TArray<FGemRuntimeInstance> EquippedGems;
	for (const FGemRuntimeInstance& Gem : OwnedGems)
	{
		if (Gem.bEquipped)
		{
			EquippedGems.Add(Gem);
		}
	}

	EquippedGems.Sort([](const FGemRuntimeInstance& A, const FGemRuntimeInstance& B)
	{
		return A.EquippedSlotIndex < B.EquippedSlotIndex;
	});

	return EquippedGems;
}

FGemRuntimeInstance* UGemInventoryComponent::FindGem(FGemRuntimeId RuntimeId)
{
	return OwnedGems.FindByPredicate([RuntimeId](const FGemRuntimeInstance& Gem)
	{
		return Gem.RuntimeId == RuntimeId;
	});
}

const FGemRuntimeInstance* UGemInventoryComponent::FindGem(FGemRuntimeId RuntimeId) const
{
	return OwnedGems.FindByPredicate([RuntimeId](const FGemRuntimeInstance& Gem)
	{
		return Gem.RuntimeId == RuntimeId;
	});
}

void UGemInventoryComponent::BroadcastChanged()
{
	OnGemInventoryChanged.Broadcast(this);
}
