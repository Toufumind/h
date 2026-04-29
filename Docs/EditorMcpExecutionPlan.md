# SpriteAssemble Editor MCP Execution Plan

## Current Goal

Use UE-MCP as the editor automation bridge for the remaining content-side vertical slice work.

The C++ runtime architecture is already compiled. The editor phase should create and wire assets, maps, actors, widgets, and validation scenes without adding gameplay rules back into Blueprints.

## MCP Setup Status

Configured files:

```text
Plugins/UE_MCP_Bridge/
.mcp.json
.ue-mcp.json
ue-mcp.yml
SpriteAssemble.uproject
```

Enabled project plugins:

```text
PythonScriptPlugin
UE_MCP_Bridge
Niagara
EnhancedInput
```

Enabled MCP categories:

```text
project
editor
reflection
level
blueprint
asset
material
widget
gameplay
niagara
feedback
```

Disabled for now:

```text
landscape
pcg
foliage
audio
gas
networking
demo
```

Reason:

```text
The vertical slice needs content assembly and runtime validation first.
Large optional systems stay disabled until the project actually needs them.
```

## Editor Connection Gate

Before using MCP tools:

```text
1. Restart or open the Unreal Editor for SpriteAssemble.uproject.
2. Confirm UE_MCP_Bridge loads.
3. Confirm MCP connection with project get_status.
4. Confirm reflection can see SpriteAssemble C++ classes.
```

If the bridge is not visible:

```text
1. Rebuild SpriteAssembleEditor.
2. Reopen the editor.
3. Check that PythonScriptPlugin and UE_MCP_Bridge are enabled.
```

### UE-MCP Ready-State Troubleshooting

The bridge can listen on `ws://localhost:9877` before the editor has finished creating a usable editor world. Older bridge behavior returned `Editor is still initializing. Please wait and retry.` forever if the startup ready ticker missed its world-context check. The project-local UE_MCP_Bridge now performs a lazy game-thread readiness check on every request, so a missed startup ticker can recover once `GEditor`, `GEngine`, and at least one `WorldContext.World()` exist.

Use a normal visible editor launch for MCP validation:

```powershell
$env:NuGetAudit='false'
$env:NUGET_AUDIT='false'
Start-Process 'D:\Projects\UE\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe' `
  -ArgumentList @('D:\Projects\Git\h\SpriteAssemble.uproject','/Game/Maps/M_RuntimeValidation','-NoSplash')
```

Avoid `Start-Process -WindowStyle Hidden` for MCP validation. In this project it allowed the socket to listen, but the editor log stayed on frame 0 and MCP requests timed out because the game-thread ticker did not progress.

Minimal MCP readiness probe:

```python
import asyncio, json, websockets

async def call(method, params=None, timeout=60):
    async with websockets.connect("ws://localhost:9877", compression=None) as ws:
        await ws.send(json.dumps({"jsonrpc": "2.0", "id": 1, "method": method, "params": params or {}}))
        return await asyncio.wait_for(ws.recv(), timeout=timeout)

print(asyncio.run(call("get_current_level")))
```

Read-only map validation probe:

```python
import unreal

world = unreal.EditorLevelLibrary.get_editor_world()
ws = world.get_world_settings()
print("MAP", world.get_name())
print("GameModeOverride", ws.get_editor_property("default_game_mode"))

smoke_cls = unreal.load_object(None, "/Script/SpriteAssemble.ValidationSmokeTestActor")
print("ValidationSmokeTestActorClass", smoke_cls)
for actor in unreal.EditorLevelLibrary.get_all_level_actors():
    if smoke_cls and actor.get_class() == smoke_cls:
        print("ValidationSmokeTestActor", actor.get_actor_label(), actor.get_path_name())
        print("  RewardPool", actor.get_editor_property("RewardPool"))
        print("  CommandOwner", actor.get_editor_property("CommandOwner"))
        print("  NodeId", actor.get_editor_property("NodeId"))
        print("  RewardOptionIndex", actor.get_editor_property("RewardOptionIndex"))
```

## Content Work Order

### E1 - Runtime Validation Map

Create or update a small validation map:

```text
/Game/Maps/M_RuntimeValidation
```

Required actors:

```text
Player actor using ASpriteAssembleCharacter
Encounter manager actor with UEncounterManagerComponent
Three spawn point actors with UEncounterSpawnPointComponent
Optional camera/light/debug floor actors
```

Acceptance:

```text
Map loads.
Components exist on placed actors.
Spawn points register with EncounterManagerComponent.
```

### E2 - Minimal Data Assets

Create data assets:

```text
/Game/Data/IceMirror/DA_IceMirror_Default
/Game/Data/EnemyActions/DA_Action_Melee_Basic
/Game/Data/EnemyActions/DA_Action_Ranged_Basic
/Game/Data/Enemies/DA_Enemy_BasicMelee
/Game/Data/Enemies/DA_Enemy_BasicRanged
/Game/Data/Waves/DA_Wave_Validation_01
/Game/Data/Encounters/DA_Encounter_Validation
/Game/Data/Gems/DA_Gem_Split
/Game/Data/Gems/DA_Gem_Pierce
/Game/Data/Gems/DA_Gem_Poison
/Game/Data/Rewards/DA_RewardPool_Validation
```

Rules:

```text
Data assets reference classes and data only.
They must not contain hidden gameplay flow rules.
Reward options produce commands.
Gems mutate specs through UGemEffect only.
```

### E3 - Blueprint Composition

Create or update thin Blueprint wrappers:

```text
/Game/Blueprints/Player/BP_Player_Composition
/Game/Blueprints/Enemies/BP_Enemy_Melee_Composition
/Game/Blueprints/Enemies/BP_Enemy_Ranged_Composition
/Game/Blueprints/Managers/BP_EncounterManager_Composition
```

Rules:

```text
Blueprints assign DataAssets and visual components.
Blueprints do not calculate damage.
Blueprints do not mutate inventory directly.
Blueprints do not own run flow.
```

### E4 - Run Flow Smoke Test

Test chain:

```text
UICommandController or debug command selects a node
RunFlow emits StartEncounter
EncounterManager starts configured encounter
WaveDirector spawns enemies
IceMirror fires shots
ShotWorldSubsystem resolves hits
Enemy death advances encounter
EncounterCompleted triggers BuildRewardOptions
Reward selection emits ClaimReward
RunState mutates through command processors
```

Acceptance:

```text
No ProjectileActor is spawned for normal shots.
No direct UI mutation is needed.
No Blueprint calls concrete gameplay components except as composition/configuration.
```

### E5 - UI Skeleton

Create minimal widgets:

```text
Health HUD bound to HealthViewModelComponent
Run HUD bound to RunViewModelWorldSubsystem
Reward selection panel bound to RewardViewModelWorldSubsystem
```

Rules:

```text
Widgets read ViewModels.
Widgets send commands through UICommandControllerComponent.
Widgets do not call HealthComponent, GemInventoryComponent, or RunStateWorldSubsystem mutators.
```

### E6 - Presentation Pass

Wire only visual feedback:

```text
Hit flash via UHitFlashComponent
Shot impact via UShotImpactPresenterWorldSubsystem
Shot visual material/mesh on AShotInstancedMeshView
Optional Niagara impact effect
```

Rules:

```text
Presentation reacts to events/delegates.
Presentation does not decide damage, target selection, rewards, or run phase.
```

## Dependency Direction During Editor Work

```text
Blueprint/DataAsset -> C++ module configuration
UI Widget -> ViewModel -> Core command/event
Presentation Blueprint -> Presentation delegate/event
Encounter assets -> Encounter module
Enemy assets -> Enemy module
Gem assets -> Gem module
```

Forbidden:

```text
UI Widget -> HealthComponent mutation
UI Widget -> GemInventory mutation
Reward Blueprint -> GemInventory mutation
Enemy Blueprint -> RunFlow
Gem Blueprint -> ShotWorldSubsystem
Shot visual Blueprint -> CombatComponent
```

## First MCP Tasks After Editor Opens

```text
project.get_status
reflection list SpriteAssemble classes
asset create folders under /Game/Data and /Game/Blueprints
asset create minimal DataAssets
blueprint create thin composition Blueprints
level create/update M_RuntimeValidation
level place player, encounter manager, spawn points
editor save all
gameplay run PIE smoke test
```

## Current Editor Smoke Status

Completed:

```text
UE-MCP bridge is installed and reachable on ws://localhost:9877.
M_RuntimeValidation exists and is saved.
Validation DataAssets exist under /Game/Data.
DA_Wave_Validation_01 spawns two /Script/SpriteAssemble.EnemyBase actors.
DA_Encounter_Validation references DA_Wave_Validation_01.
DA_IceMirror_Default is assigned to Player_CompositionRoot.IceMirrorComponent.
EncounterManager_CompositionRoot.DefaultEncounterConfig references DA_Encounter_Validation.
SpawnPoint_Left, SpawnPoint_Center, and SpawnPoint_Right use tag Validation.
Validation_Floor blocks the player; PIE player no longer falls out of range.
PIE smoke can trigger RunFlow through Player_CompositionRoot.UICommandController.SelectNode("Validation").
SelectNode starts the encounter and spawns two EnemyBase actors.
Normal validation combat used ShotInstancedMeshView and did not spawn legacy ProjectileActor.
After combat wait, EnemyBase count reached 0.
ClaimReward(0) can be called through UICommandController.
```

Known gaps:

```text
AValidationSmokeTestActor now provides a validation-only SetRewardPool bootstrap. The RewardPool must be assigned on the M_RuntimeValidation level instance; the actor does not hardcode DA_RewardPool_Validation.
UE Python does not expose a simple WorldSubsystem getter in this environment, so RunState/Reward internals were not directly read during smoke.
M_RuntimeValidation should set WorldSettings.GameModeOverride to AValidationNoPawnGameMode to avoid auto-spawning BP_Player_C_0.
BP_Player and old PaperZD animation assets still emit missing PaperZD load warnings.
Legacy ProjectileActor code still exists for asset compatibility. ASpriteAssembleCharacter no longer binds ShootActionInput to the old manual Shoot path, and ShootPressed keeps ASpriteAssembleProjectile spawning disabled even if the callback is reintroduced.
ASpriteAssembleRangedEnemy also keeps ProjectileClass for compatibility, but its legacy ASpriteAssembleProjectile fire path is disabled by default through bEnableLegacyProjectileFire=false.
```

Validation bootstrap:

```text
Place /Script/SpriteAssemble.ValidationSmokeTestActor in /Game/Maps/M_RuntimeValidation.
Set RewardPool to /Game/Data/Rewards/DA_RewardPool_Validation on the placed actor.
Set CommandOwner to Player_CompositionRoot when editing in the level, or let it find the first actor with UUICommandControllerComponent.
RunSmoke configures RewardPool, calls SelectNode("Validation"), waits, calls ClaimReward(0), then logs a read-only snapshot.
Exec alternatives: ValidationRunSmoke, ValidationSelectNode, ValidationClaimReward, ValidationSnapshot.
Snapshot reads run state through URunViewModelWorldSubsystem and reward options through URewardViewModelWorldSubsystem.
Actor scans are assertion-only: player names/count, EnemyBase count, ShotInstancedMeshView presence, and legacy ProjectileActor presence.
P0 acceptance requires exactly one player actor with no BP_Player_C_0, no legacy ProjectileActor, ShotInstancedMeshView present, EnemyBase count returning to 0, RewardOptionsCount > 0 before ClaimReward(0), and all flow driven through UICommandController.
Legacy ProjectileActor deletion is allowed only after BP_PlayerProjectile/ASpriteAssembleProjectile references are gone from shipped and validation content, no runtime content relies on ASpriteAssembleRangedEnemy.ProjectileClass or bEnableLegacyProjectileFire, old PaperZD/BP_Player assets are migrated or removed, and repeated editor smoke runs prove normal combat uses ShotWorldSubsystem without ProjectileActor actors.
```

## One-Command Validation Entry

Worker C validation automation lives in:

```text
Scripts/RunValidationSmoke.ps1
Scripts/ValidationSmokeDriver.py
```

Run from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1
```

Useful options:

```powershell
# Use an already open editor that has UE_MCP_Bridge listening on ws://localhost:9877.
powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1 -NoLaunch

# Keep the launched editor open after the smoke run.
powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1 -KeepEditorOpen

# Override editor path when UNREAL_EDITOR_EXE is not set and the default path is wrong.
powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1 -UnrealEditorPath 'D:\Projects\UE\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe'

# Override the editor readiness budget for unusually slow launches.
powershell -ExecutionPolicy Bypass -File .\Scripts\RunValidationSmoke.ps1 -EditorReadyTimeoutSeconds 600
```

The default editor readiness timeout is 900 seconds. Keep it well above 180 seconds: low disk space warnings, first AssetRegistry scans, DerivedDataCache warmup, or large uncontrolled asset discovery can push editor startup past 180 seconds before `UE_MCP_Bridge` and editor Python are actually ready. A low-disk local run has already observed `Editor Startup Time` at 421.5 seconds, so clean disk space and cache pressure first; otherwise pass an even higher `-EditorReadyTimeoutSeconds`.

What the script does:

```text
1. Launches UnrealEditor.exe with SpriteAssemble.uproject and /Game/Maps/M_RuntimeValidation, unless -NoLaunch is passed.
2. Waits for the MCP websocket on ws://localhost:9877 within the shared editor readiness budget.
3. Retries Python preflight until it emits a VALIDATION_SMOKE marker, treating MCP handler timeouts and editor-not-ready responses as startup states until the shared deadline expires.
4. Starts PIE through MCP pie_control.
5. Calls `ValidationSmokeTestActor.ConfigureRewardPool` and `ValidationSmokeTestActor.TriggerSelectNode` in the PIE world.
6. Polls `ValidationSmokeTestActor.CaptureSnapshot` until `enemy_spawned` is observed.
7. Polls until `run_phase == Reward` and reward options are available, then calls `ValidationSmokeTestActor.TriggerClaimReward`.
8. Polls until `post_claim` reaches `MapSelection`, completed node count is at least one, enemies are gone, and reward options are cleared.
9. Stops PIE unless -SkipPieStop is passed.
```

The script does not drive game flow directly. The only runtime flow triggers remain `UICommandControllerComponent.SelectNode` and `UICommandControllerComponent.ClaimReward`, both called by `AValidationSmokeTestActor`.

Expected snapshot signals:

```text
enemy_spawned: enemy_base_count >= 2
reward_available_before_claim: run_phase == Reward and reward_options_count > 0
post_claim: run_phase == MapSelection, completed_node_count >= 1, enemy_base_count == 0, reward_options_count == 0
Always: player_count == 1, found_legacy_projectile_actor == false, has_shot_instanced_mesh_view == true
```
