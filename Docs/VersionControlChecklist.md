# Version Control Checklist

## Commit

- `SpriteAssemble.uproject` and project configuration that is meant to be shared.
- `Config/`.
- `Source/`, including module build files and all C++ source/header directories.
- `Content/`, including `Content/Data/`, `Content/Maps/`, Blueprints, materials, UI, VFX, animations, and other project assets.
- `Docs/`.
- `Plugins/UE_MCP_Bridge/` source, descriptors, content, docs, and other hand-authored plugin files.

## Do Not Commit

- Generated Unreal directories: `Binaries/`, `Intermediate/`, `Saved/`, and `DerivedDataCache/`.
- Generated plugin directories such as `Plugins/*/Binaries/`, `Plugins/*/Intermediate/`, `Plugins/*/Saved/`, and `Plugins/*/DerivedDataCache/`.
- Local IDE state such as `.vs/`, `.vscode/`, `.idea/`, `*.suo`, and `*.user`.
- Local machine or editor connection files such as `.mcp.json` and `.ue-mcp.json`.
- Logs and OS metadata.

## Content Asset Notes

- Treat `.uasset` and `.umap` files as real source assets. Commit new assets together with their referenced dependencies.
- Coordinate before changing shared maps in `Content/Maps/`, because map files are binary and difficult to merge.
- Fix up redirectors in the Unreal Editor after moving or renaming assets, then commit the resulting asset changes.
- Avoid committing temporary experiments from `Content/Developers/` unless the team intentionally wants them shared.
