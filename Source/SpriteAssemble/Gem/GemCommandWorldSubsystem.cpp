#include "GemCommandWorldSubsystem.h"
#include "GemDefinition.h"
#include "GemInventoryComponent.h"
#include "../Core/GameplayCommandDispatcher.h"

void UGemCommandWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		AddGemHandle = Dispatcher->RegisterHandler(EGameplayCommandType::AddGem, FGameplayCommandNativeHandler::CreateUObject(this, &UGemCommandWorldSubsystem::HandleAddGem));
		EquipGemHandle = Dispatcher->RegisterHandler(EGameplayCommandType::EquipGem, FGameplayCommandNativeHandler::CreateUObject(this, &UGemCommandWorldSubsystem::HandleEquipGem));
		ReorderGemsHandle = Dispatcher->RegisterHandler(EGameplayCommandType::ReorderGems, FGameplayCommandNativeHandler::CreateUObject(this, &UGemCommandWorldSubsystem::HandleReorderGems));
	}
}

void UGemCommandWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::AddGem, AddGemHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::EquipGem, EquipGemHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::ReorderGems, ReorderGemsHandle);
	}

	Super::Deinitialize();
}

FGameplayCommandResult UGemCommandWorldSubsystem::HandleAddGem(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.TargetActor ? Command.TargetActor.Get() : Command.SourceActor.Get();
	if (!TargetActor)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "GemCommandMissingTarget", "Gem command target is missing."));
	}

	UGemInventoryComponent* GemInventory = TargetActor->FindComponentByClass<UGemInventoryComponent>();
	if (!GemInventory)
	{
		return FGameplayCommandResult::Unhandled();
	}

	UGemDefinition* GemDefinition = Cast<UGemDefinition>(Command.ObjectValue);
	if (!GemDefinition)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "GemCommandMissingDefinition", "Gem definition payload is missing."));
	}

	GemInventory->AddGem(GemDefinition, FMath::Max(1, Command.IntValue));
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult UGemCommandWorldSubsystem::HandleEquipGem(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.TargetActor ? Command.TargetActor.Get() : Command.SourceActor.Get();
	UGemInventoryComponent* GemInventory = TargetActor ? TargetActor->FindComponentByClass<UGemInventoryComponent>() : nullptr;
	if (!GemInventory)
	{
		return FGameplayCommandResult::Unhandled();
	}

	FGemRuntimeId RuntimeId;
	RuntimeId.Value = Command.GuidValue;
	return GemInventory->EquipGem(RuntimeId, Command.IntValue)
		? FGameplayCommandResult::Success()
		: FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "GemEquipFailed", "Failed to equip gem."));
}

FGameplayCommandResult UGemCommandWorldSubsystem::HandleReorderGems(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.TargetActor ? Command.TargetActor.Get() : Command.SourceActor.Get();
	UGemInventoryComponent* GemInventory = TargetActor ? TargetActor->FindComponentByClass<UGemInventoryComponent>() : nullptr;
	if (!GemInventory)
	{
		return FGameplayCommandResult::Unhandled();
	}

	TArray<FGemRuntimeId> OrderedGemIds;
	for (const FGuid& Guid : Command.GuidArrayValue)
	{
		FGemRuntimeId RuntimeId;
		RuntimeId.Value = Guid;
		OrderedGemIds.Add(RuntimeId);
	}

	return GemInventory->ReorderEquippedGems(OrderedGemIds)
		? FGameplayCommandResult::Success()
		: FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "GemReorderFailed", "Failed to reorder gems."));
}

UGameplayCommandDispatcher* UGemCommandWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}
