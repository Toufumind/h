#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "ShopDefinition.h"
#include "PreparationManagerWorldSubsystem.generated.h"

class UPreparationDefinition;

UCLASS()
class SPRITEASSEMBLE_API UPreparationManagerWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Preparation")
	void OpenPreparation(UPreparationDefinition* InDefinition);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Preparation")
	const TArray<FPreparationShopItem>& GetCurrentStock() const { return CurrentStock; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Preparation")
	FGameplayCommandResult BuyItem(int32 StockIndex);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Preparation")
	FGameplayCommandResult BuyItemForActor(int32 StockIndex, AActor* Buyer);

private:
	UPROPERTY()
	TObjectPtr<UPreparationDefinition> Definition = nullptr;

	UPROPERTY()
	TArray<FPreparationShopItem> CurrentStock;

	TMap<FName, int32> PurchaseCountsByItemId;

	FGameplayCommand BuildApplyCommand(const FRewardOption& RewardOption, AActor* Buyer) const;
	void DispatchCommand(const FGameplayCommand& Command) const;
};
