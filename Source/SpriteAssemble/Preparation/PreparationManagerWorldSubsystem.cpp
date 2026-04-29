#include "PreparationManagerWorldSubsystem.h"
#include "PreparationDefinition.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Economy/EconomyServiceWorldSubsystem.h"

void UPreparationManagerWorldSubsystem::OpenPreparation(UPreparationDefinition* InDefinition)
{
	Definition = InDefinition;
	CurrentStock.Reset();
	if (Definition && Definition->Shop)
	{
		CurrentStock = Definition->Shop->Stock;
	}
}

FGameplayCommandResult UPreparationManagerWorldSubsystem::BuyItem(int32 StockIndex)
{
	return BuyItemForActor(StockIndex, nullptr);
}

FGameplayCommandResult UPreparationManagerWorldSubsystem::BuyItemForActor(int32 StockIndex, AActor* Buyer)
{
	if (!CurrentStock.IsValidIndex(StockIndex))
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "InvalidPreparationStock", "Preparation stock index is invalid."));
	}

	const FPreparationShopItem& Item = CurrentStock[StockIndex];
	const int32 PurchaseCount = PurchaseCountsByItemId.FindRef(Item.ItemId);
	const int32 Price = Item.PriceRule.GetPrice(PurchaseCount);

	UEconomyServiceWorldSubsystem* Economy = GetWorld() ? GetWorld()->GetSubsystem<UEconomyServiceWorldSubsystem>() : nullptr;
	if (!Economy || !Economy->CanAfford(Price))
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "PreparationCannotAfford", "Cannot afford preparation item."));
	}

	DispatchCommand(Economy->BuildSpendCommand(Price));
	DispatchCommand(BuildApplyCommand(Item.RewardOption, Buyer));
	PurchaseCountsByItemId.FindOrAdd(Item.ItemId) = PurchaseCount + 1;
	return FGameplayCommandResult::Success();
}

FGameplayCommand UPreparationManagerWorldSubsystem::BuildApplyCommand(const FRewardOption& RewardOption, AActor* Buyer) const
{
	FGameplayCommand Command;
	Command.SourceActor = Buyer;
	Command.TargetActor = Buyer;
	Command.IntValue = RewardOption.RewardSpec.Quantity;
	Command.NameValue = RewardOption.RewardSpec.RewardId;
	Command.RewardSpec = RewardOption.RewardSpec;
	Command.ObjectValue = RewardOption.PayloadObject;

	switch (RewardOption.ApplyType)
	{
	case ERewardApplyType::AddRedCore:
		Command.Type = EGameplayCommandType::AddRedCore;
		break;
	case ERewardApplyType::AddGem:
		Command.Type = EGameplayCommandType::AddGem;
		break;
	case ERewardApplyType::AddGemSlot:
		Command.Type = EGameplayCommandType::AddGemSlot;
		break;
	case ERewardApplyType::AddPotionMax:
		Command.Type = EGameplayCommandType::AddPotionMax;
		break;
	default:
		Command.Type = EGameplayCommandType::None;
		break;
	}

	return Command;
}

void UPreparationManagerWorldSubsystem::DispatchCommand(const FGameplayCommand& Command) const
{
	if (Command.Type == EGameplayCommandType::None)
	{
		return;
	}

	if (UGameplayCommandDispatcher* Dispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr)
	{
		Dispatcher->QueueCommand(Command);
	}
}
