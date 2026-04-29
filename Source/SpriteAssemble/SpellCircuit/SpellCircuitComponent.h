#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayCommand.h"
#include "../Core/GameplayInterfaces.h"
#include "../Core/GameplaySpecs.h"
#include "../Gem/GemTypes.h"
#include "SpellCircuitComponent.generated.h"

class UGemInventoryComponent;

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API USpellCircuitComponent : public UActorComponent, public IShotHitSpecConsumer
{
	GENERATED_BODY()

public:
	USpellCircuitComponent();

	void ModifyAttackSpec(FAttackSpec& AttackSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const;
	void ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const;
	void ModifyDamageSpec(FDamageSpec& DamageSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const;
	void HandleShotHitSpec(const FShotHitSpec& HitSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const;

	FSpellExecutionContext MakeBaseContext(ESpellTriggerType TriggerType) const;

	virtual void ConsumeShotHitSpec_Implementation(const FShotHitSpec& HitSpec, TArray<FGameplayCommand>& OutCommands) override;

private:
	UGemInventoryComponent* GetGemInventory() const;
	TArray<FGemRuntimeInstance> GetExecutableGems(const FSpellExecutionContext& Context) const;
};
