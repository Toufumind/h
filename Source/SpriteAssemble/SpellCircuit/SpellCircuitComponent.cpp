#include "SpellCircuitComponent.h"
#include "../Gem/GemDefinition.h"
#include "../Gem/GemEffect.h"
#include "../Gem/GemInventoryComponent.h"

USpellCircuitComponent::USpellCircuitComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellCircuitComponent::ModifyAttackSpec(FAttackSpec& AttackSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	Context.TriggerType = ESpellTriggerType::PreCast;
	for (const FGemRuntimeInstance& Gem : GetExecutableGems(Context))
	{
		if (Gem.Definition && Gem.Definition->Effect)
		{
			Gem.Definition->Effect->ModifyAttackSpec(AttackSpec, Context, OutCommands);
		}
		++Context.CurrentGemIndex;
		Context.NextGemIndex = Context.CurrentGemIndex;
	}
}

void USpellCircuitComponent::ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	Context.TriggerType = ESpellTriggerType::OnBuildShot;
	for (const FGemRuntimeInstance& Gem : GetExecutableGems(Context))
	{
		if (Gem.Definition && Gem.Definition->Effect)
		{
			Gem.Definition->Effect->ModifyShotSpawnSpec(ShotSpawnSpec, Context, OutCommands);
		}
		++Context.CurrentGemIndex;
		Context.NextGemIndex = Context.CurrentGemIndex;
	}
}

void USpellCircuitComponent::ModifyDamageSpec(FDamageSpec& DamageSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	Context.TriggerType = ESpellTriggerType::OnBuildDamage;
	for (const FGemRuntimeInstance& Gem : GetExecutableGems(Context))
	{
		if (Gem.Definition && Gem.Definition->Effect)
		{
			Gem.Definition->Effect->ModifyDamageSpec(DamageSpec, Context, OutCommands);
		}
		++Context.CurrentGemIndex;
		Context.NextGemIndex = Context.CurrentGemIndex;
	}
}

void USpellCircuitComponent::HandleShotHitSpec(const FShotHitSpec& HitSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	Context.TriggerType = ESpellTriggerType::OnShotHit;
	for (const FGemRuntimeInstance& Gem : GetExecutableGems(Context))
	{
		if (Gem.Definition && Gem.Definition->Effect)
		{
			Gem.Definition->Effect->HandleShotHitSpec(HitSpec, Context, OutCommands);
		}
		++Context.CurrentGemIndex;
		Context.NextGemIndex = Context.CurrentGemIndex;
	}
}

FSpellExecutionContext USpellCircuitComponent::MakeBaseContext(ESpellTriggerType TriggerType) const
{
	FSpellExecutionContext Context;
	Context.TriggerType = TriggerType;
	Context.SourceActor = GetOwner();
	Context.InstigatorActor = GetOwner();
	Context.EventId = FGuid::NewGuid();

	if (UGemInventoryComponent* GemInventory = GetGemInventory())
	{
		for (const FGemRuntimeInstance& Gem : GemInventory->GetEquippedGemsInOrder())
		{
			Context.EquippedGemRuntimeIds.Add(Gem.RuntimeId);
		}
	}

	return Context;
}

void USpellCircuitComponent::ConsumeShotHitSpec_Implementation(const FShotHitSpec& HitSpec, TArray<FGameplayCommand>& OutCommands)
{
	FSpellExecutionContext Context = MakeBaseContext(ESpellTriggerType::OnShotHit);
	Context.SourceShotId = HitSpec.ShotId;
	Context.CurrentGemIndex = HitSpec.NextGemIndex;
	Context.NextGemIndex = HitSpec.NextGemIndex;
	HandleShotHitSpec(HitSpec, Context, OutCommands);
}

UGemInventoryComponent* USpellCircuitComponent::GetGemInventory() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UGemInventoryComponent>() : nullptr;
}

TArray<FGemRuntimeInstance> USpellCircuitComponent::GetExecutableGems(const FSpellExecutionContext& Context) const
{
	UGemInventoryComponent* GemInventory = GetGemInventory();
	if (!GemInventory)
	{
		return {};
	}

	TArray<FGemRuntimeInstance> EquippedGems = GemInventory->GetEquippedGemsInOrder();
	if (Context.CurrentGemIndex <= 0)
	{
		return EquippedGems;
	}

	if (Context.CurrentGemIndex >= EquippedGems.Num())
	{
		return {};
	}

	EquippedGems.RemoveAt(0, Context.CurrentGemIndex);
	return EquippedGems;
}
