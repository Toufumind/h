# Playable Vertical Slice

## Current Entry

```text
Default map: /Game/Maps/M_RuntimeValidation
Default game mode: /Script/SpriteAssemble.PlayableRunGameMode
Placed player: Player_CompositionRoot
```

`APlayableRunGameMode` does not spawn a duplicate default pawn. It possesses the placed
`ASpriteAssembleCharacter` in the map and uses:

```text
APlayableRunPlayerController
APlayableRunHUD
```

## Controls

```text
A/D or Left/Right: move
Space or W: jump
J or Left Mouse Button: fire a manual ShotWorldSubsystem shot
T: spawn a basic melee test enemy in front of the player
Enter: start the next encounter node
1: claim reward option 1
2: claim reward option 2
3: claim reward option 3
R: restart M_RuntimeValidation
```

## 3C Runtime Components

```text
UPlayerMotorComponent:
- horizontal movement facing
- coyote time
- jump buffering
- edge forgiveness ground probe
- landed feedback event

UDamageReactionComponent:
- post-hit invincibility frames
- knockback
- damage reaction event
- death reaction state
```

The controller still drives gameplay through `UUICommandControllerComponent`:

```text
Enter -> SelectNode("Validation")
1/2/3 -> ClaimReward(index)
```

## On-Screen HUD

`APlayableRunHUD` draws a lightweight debug-play HUD:

```text
Run phase
Red Core
Gem slots
Completed node count
Reward options when available
Current player action hint
```

This is intentionally code-only so the game has a playable loop without depending on
unfinished UMG assets. Replace it with proper UMG once the visual direction is ready.

## Verified Loop

The current automated smoke test validates:

```text
one placed player
no BP_Player_C_0 duplicate
encounter starts from UICommandController
two EnemyBase actors spawn
ShotInstancedMeshView is used
legacy ProjectileActor is not spawned
reward options appear before claim
ClaimReward returns to MapSelection
CompletedNodeCount increases
RedCore increases
```
