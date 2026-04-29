# SpriteAssemble Runtime Setup Checklist

This checklist covers the remaining editor/content work after the C++ architecture skeleton is in place.

Editor automation setup is tracked in `Docs/EditorMcpExecutionPlan.md`.

## Required Level Setup

```text
1. Place or keep one player actor using ASpriteAssembleCharacter.
2. Place one actor with UEncounterManagerComponent.
3. Assign EncounterManagerComponent.DefaultEncounterConfig.
4. Place spawn point actors and add UEncounterSpawnPointComponent to each.
5. Set UEncounterSpawnPointComponent.SpawnPointTag if waves need tagged spawn points.
```

## Required Data Assets

```text
Enemy:
- UEnemyDefinition for each enemy type.
- UEnemyActionDefinition for melee and ranged actions.

Encounter:
- UWaveConfig with spawn entries.
- UEncounterConfig with ordered waves.

IceMirror:
- UIceMirrorDefinition assigned on the player IceMirrorComponent.

Reward:
- Optional URewardPoolDefinition for normal gameplay.
- Preferred content path: URunConfigDefinition -> ConfigObjects[RewardPool] -> URewardPoolDefinition.
- Validation RunConfig asset: /Game/Data/Run/DA_RunConfig_Validation, with RewardPool set to /Game/Data/Rewards/DA_RewardPool_Validation.
- If no reward pool is assigned, core RewardManager fallback RedCore/GemSlot rewards are generated.
- M_RuntimeValidation still uses AValidationSmokeTestActor to inject a level-assigned RewardPool through RewardManagerWorldSubsystem.SetRewardPool for smoke/bootstrap compatibility.
- The validation actor does not hardcode DA_RewardPool_Validation; keep the placed level instance assignment intact until the validation map is explicitly migrated to RunConfig.

Preparation:
- Optional UPreparationDefinition.
- Optional UShopDefinition with FPreparationShopItem entries.
```

## Required Blueprint Bindings

```text
Playable entry:
- Default map is /Game/Maps/M_RuntimeValidation.
- Default game mode is /Script/SpriteAssemble.PlayableRunGameMode.
- APlayableRunGameMode possesses the placed Player_CompositionRoot and uses APlayableRunHUD.
- Current playable controls: A/D or Left/Right moves, Space/W jumps, J/Left Mouse Button fires a manual ShotWorldSubsystem shot, T spawns a basic melee test enemy, Enter starts the Validation encounter node, 1/2/3 claim reward options, and R restarts M_RuntimeValidation.

Player:
- Ensure ASpriteAssembleCharacter has the generated components visible after recompile.
- Ensure PlayerMotorComponent and DamageReactionComponent are present on ASpriteAssembleCharacter.
- PlayerMotorComponent owns jump buffering, coyote time, edge forgiveness, facing, and landed feedback.
- DamageReactionComponent owns invincibility frames, knockback, damage reaction state, and death reaction state.
- Assign IceMirrorComponent.Definition.
- Add gem definitions through Reward/Preparation or test commands.

Enemy:
- Existing legacy SpriteAssembleEnemyBase blueprints are now targetable/damageable.
- New enemies should preferably derive from AEnemyBase.
- For AEnemyBase, assign EnemyDefinition.
- For quick 3C testing, press T in PIE to spawn AEnemyBase with /Game/Data/Enemies/DA_Enemy_BasicMelee.

Shot:
- AShotInstancedMeshView now uses /Engine/BasicShapes/Cube by default so shots remain visible from the 2D side camera.
- Replace with a pixel-art mesh/material later for final visuals.

UI:
- Bind widgets to HealthViewModelComponent, RunViewModelWorldSubsystem, and RewardViewModelWorldSubsystem.
- Send user actions through UICommandControllerComponent.

Presentation:
- Bind hit flash visuals to UHitFlashComponent.OnFlashStateChanged.
- Bind landing VFX/SFX to UPlayerMotorComponent.OnLandedFeedback.
- Bind death UI/animation/SFX to UDamageReactionComponent.OnDeathReaction or HealthComponent.OnDeath.
- Bind shot impact VFX to UShotImpactPresenterWorldSubsystem.OnShotImpactPresented.
```

## Known Technical Follow-Ups

```text
Collision:
- Shot sweep currently uses ECC_Pawn.
- AreaEffect overlap currently uses ECC_Pawn.
- Configure project collision channels before content-heavy tuning.

Legacy:
- Old ProjectileActor path still exists for Blueprint/asset compatibility only.
- ASpriteAssembleCharacter no longer binds ShootActionInput to the legacy manual Shoot callbacks.
- ShootPressed also keeps ASpriteAssembleProjectile spawning disabled, so player input/default validation flow must not create ProjectileActor instances.
- ASpriteAssembleRangedEnemy keeps ProjectileClass for legacy asset compatibility, but legacy projectile fire is disabled by default through bEnableLegacyProjectileFire=false.
- ASpriteAssembleProjectile files and ProjectileClass properties are retained until legacy Blueprint assets are migrated or deleted.
- Delete conditions: no shipped/test map references BP_PlayerProjectile or ASpriteAssembleProjectile, no Blueprint defaults require ProjectileClass or bEnableLegacyProjectileFire for runtime combat, validation has repeatedly passed with ShotWorldSubsystem/ShotInstancedMeshView only, and old PaperZD/BP_Player assets are either migrated to composition wrappers or removed.
- Legacy PaperZD-era content has been moved out of active Content scanning to `DeprecatedContentBackup/LegacyPaperZDAssets_20260428`.
- Active validation content should use `/Game/Maps/M_RuntimeValidation`, `/Game/Data/*`, and C++ composition actors instead of old `BP_Player`, `ABP_Player`, `BP_CrabMelee`, `BP_FlyeyeRanged`, `BP_PlayerProjectile`, or `First_Map`.

Performance:
- Shot visual sync currently clears/rebuilds ISM instances each tick.
- Replace with stable instance updates before large bullet counts.

MCP:
- UE-MCP bridge plugin is installed in Plugins/UE_MCP_Bridge.
- Restart the editor after rebuild before using MCP editor tools.
- Keep editor-side automation inside the dependency rules in Docs/EditorMcpExecutionPlan.md.

Validation:
- M_RuntimeValidation should use AValidationNoPawnGameMode in WorldSettings so PIE does not auto-spawn the project default BP_Player_C_0.
- Keep one placed Player_CompositionRoot as the command owner.
- Use `Scripts/RunValidationSmoke.ps1` for the full smoke path. It avoids `RunSmoke`'s fixed delayed claim and instead calls `ConfigureRewardPool`, `TriggerSelectNode`, repeated `CaptureSnapshot`, and `TriggerClaimReward`.
- Preferred one-command entry: `powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1`.
- Use `-NoLaunch` when the editor is already open and UE_MCP_Bridge is already listening on `ws://localhost:9877`.
- Default editor readiness timeout is 900 seconds. Low disk space warnings, first AssetRegistry scans, DerivedDataCache warmup, and uncontrolled asset discovery can make editor startup exceed 180 seconds; a low-disk run has observed 421.5 seconds. Clean disk/cache pressure first, or pass a higher `-EditorReadyTimeoutSeconds` if the machine remains slow.
- The validation actor must only start flow through UICommandControllerComponent.SelectNode and claim rewards through UICommandControllerComponent.ClaimReward.
- Startup layer: normal runtime content should assign RewardPool through RunConfig so RewardManagerWorldSubsystem.ResolveRewardPool finds ConfigObjects[RewardPool] before any override. The validation actor's SetRewardPool path is retained only for smoke/bootstrap compatibility; if its RewardPool is empty, it logs a warning and does not inject a fallback pool.
- Driver layer: call UICommandControllerComponent.SelectNode and UICommandControllerComponent.ClaimReward only.
- Assertion layer: count ASpriteAssembleCharacter and AEnemyBase, list player actor names, detect AShotInstancedMeshView, flag legacy ASpriteAssembleProjectile, and read Run/Reward state only through URunViewModelWorldSubsystem and URewardViewModelWorldSubsystem.
- P0 acceptance: one player actor name only, no BP_Player_C_0, SelectNode("Validation") starts the encounter, two EnemyBase actors can spawn and later reach zero, ShotInstancedMeshView exists, legacy ProjectileActor is not found, RewardOptionsCount is greater than zero before ClaimReward(0), and ClaimReward(0) is driven through UICommandController.
```
