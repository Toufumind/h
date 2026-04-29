import argparse
import json
import sys

import unreal


SMOKE_CLASS_PATH = "/Script/SpriteAssemble.ValidationSmokeTestActor"
MAP_PATH_DEFAULT = "/Game/Maps/M_RuntimeValidation"


def emit(kind, **fields):
    payload = {"kind": kind}
    payload.update(fields)
    unreal.log("VALIDATION_SMOKE " + json.dumps(payload, sort_keys=True))
    print("VALIDATION_SMOKE " + json.dumps(payload, sort_keys=True))


def fail(message, **fields):
    emit("error", message=message, **fields)
    raise RuntimeError(message)


def load_smoke_class():
    smoke_cls = unreal.load_class(None, SMOKE_CLASS_PATH)
    if not smoke_cls:
        fail("ValidationSmokeTestActor class was not found", class_path=SMOKE_CLASS_PATH)
    return smoke_cls


def get_editor_world():
    subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    if subsystem and hasattr(subsystem, "get_editor_world"):
        world = subsystem.get_editor_world()
        if world:
            return world
    if hasattr(unreal.EditorLevelLibrary, "get_editor_world"):
        return unreal.EditorLevelLibrary.get_editor_world()
    return None


def get_game_world():
    subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    if subsystem and hasattr(subsystem, "get_game_world"):
        world = subsystem.get_game_world()
        if world:
            return world
    if hasattr(unreal.EditorLevelLibrary, "get_game_world"):
        world = unreal.EditorLevelLibrary.get_game_world()
        if world:
            return world
    return None


def get_world(prefer_pie):
    if prefer_pie:
        world = get_game_world()
        if world:
            return world
    return get_editor_world()


def get_actors(world, actor_class):
    if world:
        return list(unreal.GameplayStatics.get_all_actors_of_class(world, actor_class))
    return []


def find_smoke_actor(world):
    smoke_cls = load_smoke_class()
    actors = get_actors(world, smoke_cls)
    if not actors:
        fail("No ValidationSmokeTestActor found in the current world")
    if len(actors) > 1:
        emit("warning", message="Multiple ValidationSmokeTestActor instances found; using the first", count=len(actors))
    return actors[0]


def snapshot_to_dict(snapshot):
    names = {
        "player_count": ("player_count",),
        "enemy_base_count": ("enemy_base_count",),
        "has_shot_instanced_mesh_view": ("b_has_shot_instanced_mesh_view", "has_shot_instanced_mesh_view"),
        "player_has_player_motor_component": (
            "b_player_has_player_motor_component",
            "player_has_player_motor_component",
        ),
        "player_has_damage_reaction_component": (
            "b_player_has_damage_reaction_component",
            "player_has_damage_reaction_component",
        ),
        "player_has_visible_marker_plane": (
            "b_player_has_visible_marker_plane",
            "player_has_visible_marker_plane",
        ),
        "enemy_has_visible_marker_plane": (
            "b_enemy_has_visible_marker_plane",
            "enemy_has_visible_marker_plane",
        ),
        "player_horizontal_speed": ("player_horizontal_speed",),
        "player_is_falling": ("b_player_is_falling", "player_is_falling"),
        "player_is_jump_buffered": ("b_player_is_jump_buffered", "player_is_jump_buffered"),
        "player_is_using_coyote_time": (
            "b_player_is_using_coyote_time",
            "player_is_using_coyote_time",
        ),
        "player_is_invincible": ("b_player_is_invincible", "player_is_invincible"),
        "player_is_dead": ("b_player_is_dead", "player_is_dead"),
        "found_legacy_projectile_actor": ("b_found_legacy_projectile_actor", "found_legacy_projectile_actor"),
        "command_owner_name": ("command_owner_name",),
        "player_actor_names": ("player_actor_names",),
        "run_phase": ("run_phase",),
        "completed_node_count": ("completed_node_count",),
        "red_core": ("red_core",),
        "gem_slot_count": ("gem_slot_count",),
        "reward_options_count": ("reward_options_count",),
        "reward_option_names": ("reward_option_names",),
    }
    array_fields = {"player_actor_names", "reward_option_names"}
    result = {}
    for output_name, candidates in names.items():
        value = None
        found = False
        for name in candidates:
            try:
                value = getattr(snapshot, name)
                found = True
                break
            except Exception:
                continue
        if not found:
            continue
        if output_name in array_fields:
            result[output_name] = [str(item) for item in value]
        else:
            result[output_name] = str(value) if not isinstance(value, (int, float, bool)) else value
    return result


def command_preflight(map_path):
    world = get_world(prefer_pie=False)
    if not world:
        fail("Editor world is not available")

    smoke_actor = find_smoke_actor(world)
    reward_pool = smoke_actor.get_editor_property("RewardPool")
    command_owner = smoke_actor.get_editor_property("CommandOwner")
    emit(
        "preflight",
        world=world.get_name(),
        expected_map=map_path,
        smoke_actor=smoke_actor.get_actor_label(),
        reward_pool=reward_pool.get_path_name() if reward_pool else "",
        command_owner=command_owner.get_actor_label() if command_owner else "",
        node_id=str(smoke_actor.get_editor_property("NodeId")),
        reward_option_index=int(smoke_actor.get_editor_property("RewardOptionIndex")),
    )


def command_start():
    world = get_world(prefer_pie=True)
    if not world:
        fail("PIE game world is not available")

    smoke_actor = find_smoke_actor(world)
    configured = smoke_actor.configure_reward_pool()
    smoke_actor.trigger_select_node()
    emit("start", world=world.get_name(), smoke_actor=smoke_actor.get_actor_label(), reward_pool_configured=bool(configured))


def command_claim():
    world = get_world(prefer_pie=True)
    if not world:
        fail("PIE game world is not available")

    smoke_actor = find_smoke_actor(world)
    smoke_actor.trigger_claim_reward()
    emit("claim", world=world.get_name(), smoke_actor=smoke_actor.get_actor_label())


def command_snapshot():
    world = get_world(prefer_pie=True)
    if not world:
        fail("PIE game world is not available")

    smoke_actor = find_smoke_actor(world)
    snapshot = smoke_actor.capture_snapshot()
    emit("snapshot", world=world.get_name(), smoke_actor=smoke_actor.get_actor_label(), snapshot=snapshot_to_dict(snapshot))


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--command", choices=["preflight", "start", "claim", "snapshot"], required=True)
    parser.add_argument("--map", default=MAP_PATH_DEFAULT)
    args = parser.parse_args(argv)

    if args.command == "preflight":
        command_preflight(args.map)
    elif args.command == "start":
        command_start()
    elif args.command == "claim":
        command_claim()
    elif args.command == "snapshot":
        command_snapshot()


if __name__ == "__main__":
    main(sys.argv[1:])
