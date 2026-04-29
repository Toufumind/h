# Legacy Asset Cleanup

## 2026-04-28 PaperZD-era quarantine

The old PaperZD-era assets were moved out of active UE `Content/` scanning:

```text
DeprecatedContentBackup/LegacyPaperZDAssets_20260428/
```

Moved paths:

```text
Content/Animations/
Content/Blueprint/Character/
Content/Blueprint/BP_Player.uasset
Content/Blueprint/Projectile/BP_PlayerProjectile.uasset
Content/Map/First_Map.umap
```

Reason:

```text
These assets produced unknown custom version / old animation asset warnings and are no longer part of
the validation runtime path. Keeping them outside Content preserves a local recovery copy while stopping
AssetRegistry scans from loading stale Blueprint and animation packages.
```

Current active validation path:

```text
Content/Maps/M_RuntimeValidation.umap
Content/Data/**
Source/SpriteAssemble/** composition modules
```

Deletion condition:

```text
Delete DeprecatedContentBackup only after confirming no shipped map, test map, or design reference still
needs the old PaperZD player/enemy animation assets or BP_PlayerProjectile.
```
