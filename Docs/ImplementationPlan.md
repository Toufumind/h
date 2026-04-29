# SpriteAssemble Implementation Plan

## 1. Purpose

This document turns `Docs/Architecture.md` into an implementation sequence.

It is not a replacement for the architecture contract. If this plan conflicts with the architecture document, the architecture document wins.

Primary goal:

```text
Build a playable vertical slice without reintroducing coupling.
```

First playable target:

```text
One arena combat
Player can move, dash, take damage, use potion
IceMirror auto-targets enemies and fires shots
Shots use UShotWorldSubsystem + Instanced Mesh
Gems modify Specs through SpellCircuit
Enemies spawn in waves
Combat ends with reward selection
Reward updates run state through Commands
```

## 2. Non-Negotiable Rules

These rules must hold during implementation:

```text
No ProjectileActor for normal shots.
No gem logic in PlayerCharacter.
No shot logic in PlayerCharacter.
No direct UI-to-gameplay-component mutation.
No Reward/Preparation direct mutation of GemInventory or Attribute.
No CommandDispatcher gameplay rules.
No RunFlow content rules.
No Shot array index exposed outside UShotWorldSubsystem.
No gameplay module hard-loading heavy presentation assets.
```

## 3. Implementation Phases

```text
P0: Core contracts and command/event base
P1: Attribute, Health, Combat
P2: ShotWorldSubsystem with Instanced Mesh
P3: IceMirror auto-cast
P4: GemInventory + SpellCircuit + first gems
P5: Enemy + Encounter/Wave
P6: RunFlow + Reward
P7: UI ViewModels
P8: Preparation/Shop/Workshop
P9: Presentation polish
```

## 3.1 Current Implementation Status

Implemented:

```text
P0 Core contracts and command/event base
P1 Attribute, Health, Combat
P2 ShotWorldSubsystem with Instanced Mesh
P3 IceMirror auto-cast skeleton
P4 GemInventory + SpellCircuit + first gem effects
P5 Enemy + Encounter/Wave skeleton
Effect command processor and basic status effect runtime
AreaEffect command processor and batched area effect runtime
Enemy action phase state machine
Shared ShooterComponent and ranged enemy shot path
EncounterSpawnPointComponent level-side spawn registration
P6 RunState + RunFlow + Reward skeleton
P7 UI ViewModel + UICommandController skeleton
P8 Economy + Preparation skeleton
P9 Presentation skeleton
Command processors for Combat and GemInventory
PlayerCharacter and legacy EnemyBase adapter integration
```

P5 current scope:

```text
EnemyDefinition defines enemy tuning data.
EnemyBase composes Attribute, Health, Combat, Brain, and Action components.
EnemyBase implements IDamageable, ITargetable, and ITeamAgent.
EnemyBrainComponent selects target through Core interfaces.
EnemyActionComponent chases and attacks through Core interfaces.
EncounterConfig and WaveConfig define wave data.
EncounterManagerComponent starts waves, tracks alive enemies, and emits EncounterCompleted.
Encounter does not open Reward, mutate ChapterMap, or depend on UI.
ShotWorldSubsystem sweeps moved shots, builds FShotHitSpec, lets Core shot-hit consumers react, and resolves damage through Combat.
SpellCircuit implements the Core shot-hit consumer interface, so Shot does not include Gem or SpellCircuit headers.
PoisonGemEffect emits ApplyStatusEffect commands.
EffectCommandWorldSubsystem owns ApplyStatusEffect command routing.
EffectComponent owns active status effects and periodic damage.
AreaEffectWorldSubsystem owns CreateAreaEffect command routing.
AreaEffectWorldSubsystem owns delayed/persistent area instances and applies radius damage through Core/Combat contracts.
EnemyActionComponent advances Telegraph, Startup, Active, Recovery, and Cooldown phases.
EnemyActionDefinition supplies action timing, range, and damage data.
Enemy attacks apply damage only during the Active phase through IDamageable.
ShooterComponent builds and submits FShotSpawnSpec to UShotWorldSubsystem.
Ranged enemy actions use ShooterComponent instead of ProjectileActor.
EncounterSpawnPointComponent lets any level actor register itself as an encounter spawn point.
RunStateWorldSubsystem owns run phase, RedCore, gem slot count, potion max, completed node count, and pending rewards.
RunConfigDefinition and RunConfigWorldSubsystem provide the run-scoped runtime configuration entry point.
RunFlowWorldSubsystem listens to NodeSelected, EncounterCompleted, RewardClaimed, and PreparationExited events.
RunFlow dispatches StartEncounter, BuildRewardOptions, and CompleteNode commands without owning content rules.
RewardManagerWorldSubsystem owns reward option building and reward claiming.
RewardManagerWorldSubsystem resolves its RewardPool through RunConfig first, with legacy direct injection retained only as a compatibility fallback.
Validation content now includes /Game/Data/Run/DA_RunConfig_Validation with ConfigObjects[RewardPool] pointing at /Game/Data/Rewards/DA_RewardPool_Validation.
Reward claim emits owner commands such as AddRedCore, AddGemSlot, or AddPotionMax.
HealthViewModelComponent exposes health snapshots without mutating HealthComponent.
RunViewModelWorldSubsystem exposes RunState snapshots.
RewardViewModelWorldSubsystem exposes RewardManager options.
UICommandControllerComponent sends ClaimReward, SelectNode, and PreparationExited commands/events.
EconomyServiceWorldSubsystem validates RedCore affordability and builds spend commands.
PreparationManagerWorldSubsystem opens preparation stock and converts purchases into spend/apply commands.
ShopDefinition and PreparationDefinition define preparation data without depending on UI widgets.
PresentationComponent exposes FPresentationState through IPresentationStateProvider.
HitFlashComponent listens to Health damage and emits flash state only.
ShotImpactPresenterWorldSubsystem listens to ShotHit events and exposes presentation delegates.
CombatCommandWorldSubsystem owns Heal and ApplyDamage command handling.
GemCommandWorldSubsystem owns AddGem, EquipGem, and ReorderGems command handling.
SpriteAssembleCharacter now acts as a composition root for the new runtime components and implements Core target/damage/team interfaces.
Legacy SpriteAssembleEnemyBase implements Core target/damage/team interfaces so existing enemy Blueprints can be hit by the new shot path.
EncounterManagerComponent now handles StartEncounter commands using its configured DefaultEncounterConfig.
Reward and Preparation apply commands now preserve SourceActor/TargetActor so actor-owned inventories can receive AddGem.
```

Remaining P5 work:

```text
Shot collision filtering currently uses ECC_Pawn; tune object channels after level collision setup is stable.
AreaEffect overlap filtering currently uses ECC_Pawn; tune object channels after level collision setup is stable.
Status effects are currently generic DOT by Magnitude; typed poison/slow/mark behavior still needs effect definitions or tag-driven policies.
AreaEffect is gameplay-only right now; warning decals, hit VFX, and debug drawing belong to Presentation later.
Reward generation is fallback/static unless ResolveRewardPool finds a URewardPoolDefinition.
RewardPool assignment should now be authored through URunConfigDefinition using the RewardPool config key instead of relying on ValidationSmokeTestActor injection.
ValidationSmokeTestActor SetRewardPool remains only a smoke/bootstrap compatibility override and should not replace the RunConfig content path.
P6-P9 code skeletons compile; Blueprint widgets, assets, maps, VFX, and full content data still need authoring.
Legacy actor logic still exists for compatibility and should be retired gradually after Blueprint migration.
Runtime/editor setup is tracked in Docs/RuntimeSetupChecklist.md.
MCP editor execution is tracked in Docs/EditorMcpExecutionPlan.md.
```

## 4. P0 - Core Contracts

Create the shared contract layer first.

Files/modules:

```text
Source/SpriteAssemble/Core/
- GameplayIds.h
- GameplayTags.h
- GameplayCommand.h
- GameplayEvent.h
- GameplaySpecs.h
- GameplayInterfaces.h
- GameplayCommandDispatcher.h
- GameplayEventBus.h
```

Required types:

```text
FGameplayCommand
FGameplayCommandResult
FGameplayEvent
FAttackSpec
FSpellExecutionContext
FShotSpawnSpec
FDamageSpec
FShotHitSpec
FStatusEffectSpec
FAreaEffectSpec
FRewardSpec
FShotId
FGemRuntimeId
FRunNodeId
```

Required interfaces:

```text
IDamageable
ITargetable
ITeamAgent
ICommandReceiver
IPresentationStateProvider
```

Acceptance:

```text
Specs contain no UHealthComponent*, UGemInventoryComponent*, or UAttributeComponent*.
Commands can be queued and routed by type.
Dispatcher does not execute gameplay logic directly.
Events can be batched for UI/Presentation.
```

Review checklist:

```text
Core contains contracts only.
Core does not include Character, Enemy, UI, Presentation, or concrete gameplay components.
```

## 5. P1 - Attribute, Health, Combat

Implement basic state and damage resolution.

Files/modules:

```text
Source/SpriteAssemble/Attribute/
- AttributeComponent.h/.cpp
- AttributeTypes.h

Source/SpriteAssemble/Combat/
- HealthComponent.h/.cpp
- CombatComponent.h/.cpp
- DamageResolver.h/.cpp
```

Responsibilities:

```text
AttributeComponent owns numeric stat queries.
HealthComponent owns current health and death.
CombatComponent builds Attack/Damage specs and resolves hits.
DamageResolver applies FDamageSpec to IDamageable targets.
```

Acceptance:

```text
Player and enemy can both use HealthComponent.
DamageSpec can damage either player or enemy without casting concrete classes.
Health changes emit events.
Death emits event.
No UI code inside Combat or Health.
```

Review checklist:

```text
No Cast<ASpriteAssembleCharacter> in Combat.
No Widget references in Health.
No Presentation references in Combat.
```

## 6. P2 - ShotWorldSubsystem + Instanced Mesh

Replace normal ProjectileActor usage for standard shots.

Files/modules:

```text
Source/SpriteAssemble/Shot/
- ShotWorldSubsystem.h/.cpp
- ShotTypes.h
- ShotInstancedMeshView.h/.cpp
- ShotVisualDefinition.h/.cpp
```

Required runtime types:

```text
FShotInstance
FShotId
FShotInstanceHandle
FShotVisualType
```

ShotWorldSubsystem responsibilities:

```text
Submit FShotSpawnSpec.
Create FShotInstance.
Move shots in explicit phase.
Sweep from PreviousPosition to Position.
Generate FShotHitSpec.
Mark dead shots.
Remove dead shots after iteration.
Maintain FShotId mapping.
Notify render view.
```

ShotInstancedMeshView responsibilities:

```text
Own InstancedStaticMeshComponent.
Add visual instance.
Update visual transform.
Remove visual instance.
Maintain render-instance mapping.
```

Acceptance:

```text
One submitted shot appears as an instanced mesh.
100+ shots can exist without spawning 100 Actors.
Shots damage enemies through Combat.
Shot removal does not invalidate hit processing.
External code never receives array index as identity.
```

Review checklist:

```text
No ProjectileActor used for normal shots.
No Gem logic inside ShotWorldSubsystem.
No PlayerCharacter cast inside Shot.
Shot uses FShotId externally.
```

## 7. P3 - IceMirror Auto-Cast

Implement automatic player spell firing.

Files/modules:

```text
Source/SpriteAssemble/IceMirror/
- IceMirrorComponent.h/.cpp
- TargetingComponent.h/.cpp
- IceMirrorDefinition.h/.cpp
```

Responsibilities:

```text
IceMirrorComponent ticks attack interval.
TargetingComponent finds target by policy.
IceMirror builds base FAttackSpec.
IceMirror requests SpellCircuit execution.
IceMirror submits final FShotSpawnSpec to ShotWorldSubsystem.
```

First target policy:

```text
Nearest target in range.
```

Acceptance:

```text
Player can stand still and IceMirror automatically fires at nearest enemy.
Attack speed, range, base damage come from Attribute/IceMirror data.
If no target exists, IceMirror does not fire.
```

Review checklist:

```text
IceMirror does not know concrete enemy classes.
IceMirror does not implement gem rules.
IceMirror does not spawn Actor projectiles.
```

## 8. P4 - GemInventory + SpellCircuit

Implement magic-program behavior.

Files/modules:

```text
Source/SpriteAssemble/Gem/
- GemDefinition.h/.cpp
- GemRuntimeInstance.h
- GemInventoryComponent.h/.cpp
- GemEffect.h/.cpp
- GemEffects/SplitGemEffect.h/.cpp
- GemEffects/TripleShotGemEffect.h/.cpp
- GemEffects/PierceGemEffect.h/.cpp
- GemEffects/PoisonGemEffect.h/.cpp

Source/SpriteAssemble/SpellCircuit/
- SpellCircuitComponent.h/.cpp
- SpellExecutionContext.h
```

First gems:

```text
Split
TripleShot
Pierce
Poison
```

Execution rule:

```text
Slots execute left to right.
Generated child shots continue from NextGemIndex.
Generated child shots do not re-run earlier gems.
```

Acceptance:

```text
Changing gem order changes spell behavior.
Split before Poison makes split shots poison-capable.
Poison before Split does not poison child shots unless later chain allows it.
Split cannot infinitely split itself.
Poison applies through command/effect path, not direct Health calls.
```

Review checklist:

```text
GemEffect depends only on Core contracts and Gem data.
GemEffect does not call HealthComponent, EffectComponent, AreaEffectSubsystem, UI, or Presentation.
SpellCircuit does not own gem inventory storage.
SpellCircuit does not move shots or apply damage.
```

## 9. P5 - Enemy + Encounter/Wave

Implement the first arena combat loop.

Files/modules:

```text
Source/SpriteAssemble/Enemy/
- EnemyBase.h/.cpp
- EnemyBrainComponent.h/.cpp
- EnemyActionComponent.h/.cpp
- EnemyDefinition.h/.cpp
- EnemyActionDefinition.h/.cpp

Source/SpriteAssemble/Encounter/
- EncounterManagerComponent.h/.cpp
- WaveDirector.h/.cpp
- SpawnDirector.h/.cpp
- EncounterConfig.h/.cpp
- WaveConfig.h/.cpp
```

First enemies:

```text
Melee chaser
Ranged caster
High-health slow enemy
```

Acceptance:

```text
Encounter spawns multiple waves.
Wave ends when all spawned enemies are dead.
Encounter emits EncounterCompleted event.
Enemy attack uses telegraph/startup/active/recovery.
Ranged enemy uses Shooter/Shot path.
```

Review checklist:

```text
Enemy does not cast to PlayerCharacter.
Encounter does not open Reward directly.
Encounter does not advance ChapterMap directly.
EnemyAction owns timing, not animation blueprint.
```

## 10. P6 - RunFlow + Reward

Implement single-run orchestration and post-combat reward.

Files/modules:

```text
Source/SpriteAssemble/Run/
- RunState.h/.cpp
- RunFlowWorldSubsystem.h/.cpp

Source/SpriteAssemble/ChapterMap/
- ChapterMapState.h/.cpp
- ChapterMapDefinition.h/.cpp

Source/SpriteAssemble/Reward/
- RewardManager.h/.cpp
- RewardPoolDefinition.h/.cpp
- RewardTypes.h
```

RunFlow is fixed as:

```text
URunFlowWorldSubsystem
```

Responsibilities:

```text
Listen to NodeSelected.
Dispatch StartEncounter / OpenPreparation / OpenReward.
Listen to EncounterCompleted.
Dispatch BuildRewardOptions.
Listen to RewardClaimed.
Dispatch CompleteNode.
Return to map selection.
```

Acceptance:

```text
Selecting a combat node starts combat.
Combat completion opens reward.
Claiming reward completes node.
ChapterMap shows next available nodes.
Reward applies through command, not direct GemInventory mutation.
```

Review checklist:

```text
RunFlow does not generate rewards.
RunFlow does not spawn enemies.
RunFlow does not price shop items.
ChapterMap does not start Encounter directly.
Encounter does not open Reward directly.
Reward does not advance ChapterMap directly.
```

## 11. P7 - UI ViewModels

Implement UI without direct gameplay mutation.

Files/modules:

```text
Source/SpriteAssemble/UI/
- HealthViewModel.h/.cpp
- PotionViewModel.h/.cpp
- GemInventoryViewModel.h/.cpp
- RunViewModel.h/.cpp
- ChapterMapViewModel.h/.cpp
- RewardViewModel.h/.cpp
- UICommandController.h/.cpp
```

Acceptance:

```text
HUD displays health, potion count, RedCore, equipped gems.
Reward UI displays 3 options.
Map UI displays selectable next nodes.
UI issues commands through UICommandController.
Widgets do not call gameplay components directly.
```

Review checklist:

```text
No Widget -> HealthComponent mutation.
No Widget -> GemInventory mutation.
No Widget -> ChapterMap mutation.
UI sends command, owning system applies it.
```

## 12. P8 - Preparation / Shop / Workstation

Implement the first preparation node.

Files/modules:

```text
Source/SpriteAssemble/Economy/
- EconomyService.h/.cpp
- PriceRule.h/.cpp

Source/SpriteAssemble/Preparation/
- PreparationManager.h/.cpp
- ShopDefinition.h/.cpp
- PreparationDefinition.h/.cpp
- WorkstationService.h/.cpp
```

First functions:

```text
Gem merchant with 3 gems.
One refresh.
Gem workstation reorder.
Gem equip/replace.
Spirit statue heal or RedCore.
Craftsman basic upgrade.
```

Acceptance:

```text
Buying spends RedCore through command.
Equipping/reordering gems changes RunState through command.
Workstation order affects next combat.
Preparation exits through RunFlow.
```

Review checklist:

```text
Preparation does not directly mutate GemInventory.
Economy does not mutate Character.
Shop does not know UI widget classes.
```

## 13. P9 - Presentation Polish

Implement visual/audio response after gameplay is stable.

Files/modules:

```text
Source/SpriteAssemble/Presentation/
- PresentationComponent.h/.cpp
- HitFlashComponent.h/.cpp
- PresentationState.h
- ShotImpactPresenter.h/.cpp
```

Acceptance:

```text
Player/enemy states drive animations through presentation state.
Hit flash responds to damage events.
Shot hit VFX responds to hit events.
Gem trigger VFX responds to gem trigger events.
No Presentation code applies gameplay damage.
```

Review checklist:

```text
Presentation does not call Combat.
Presentation does not modify gem chain.
Presentation does not mutate RunState.
Gameplay modules do not hard-load heavy presentation assets.
```

## 14. First Vertical Slice Acceptance

The first vertical slice is complete when:

```text
Player enters one arena.
Enemies spawn in waves.
IceMirror auto-fires using ShotWorldSubsystem.
At least 4 gems work through SpellCircuit.
Gem order changes behavior.
Player can dash and use potion.
Enemies can damage player.
Encounter completes.
Reward UI opens.
Reward updates run state through command.
Next map node becomes selectable.
```

Architecture acceptance:

```text
No normal ProjectileActor.
No PlayerCharacter gem rules.
No UI direct component mutation.
No Reward direct GemInventory mutation.
No ChapterMap direct Encounter start.
No Encounter direct Reward open.
No RunFlow content rules.
```

## 15. Review Gates

Before merging any implementation phase, check:

```text
Does this introduce a concrete class dependency across modules?
Does this mutate state owned by another module?
Should this be a Command instead?
Should this be an Event instead?
Should this be a Spec field instead?
Is this state source-of-truth or projection?
Does this DataAsset hard-reference presentation assets from gameplay?
Does UI talk to ViewModel or gameplay component?
Does Shot expose FShotId or array index?
```

If the answer violates `Docs/Architecture.md`, stop and revise the design before adding more code.
