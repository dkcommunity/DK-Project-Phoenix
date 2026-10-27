# PhoenixFilePatcher

PhoenixFilePatcher is the Project Phoenix runtime data packer.

It produces `.phx` files for client-side protected data loading. It is not the
legacy updater `.dp2` patcher.

## Commands

GUI mode:

```bat
PhoenixFilePatcher.exe
```

In GUI mode, select an output folder first. Patched files are written under that
folder with their logical relative path preserved.

```bat
PhoenixFilePatcher.exe pack-file --input Data\Info\Action.inf --output Data\Info\Action.inf.phx --logical Data/Info/Action.inf
```

```bat
PhoenixFilePatcher.exe pack-inf --input F:\Project Phoenix\GameData --output F:\Project Phoenix\release_phx --verify
```

```bat
PhoenixFilePatcher.exe verify-file --input Data\Info\Action.inf --phx Data\Info\Action.inf.phx --logical Data/Info/Action.inf
```

## Notes

- The tool intentionally does not expose a plaintext unpack command.
- The `.phx` container uses a Phoenix-specific binary header and AES-256-GCM through Windows CNG.
- The file extension and header are not standard archive signatures.
- Client runtime lookup is handled by `ivfstream`: `X.inf` automatically tries `X.inf.phx` first.
- GUI mode does not write next to source files unless that source folder is explicitly selected as the output folder.
