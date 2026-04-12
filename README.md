# FM26 Xbox Export POC

Proof-of-concept work for making FM26 player export function on the Xbox / Microsoft Store build, where the standard BepInEx winhttp bootstrap does not load.

## What works

- External DLL injection into the running FM26 `WindowsApps` process
- Stable native bootstrap DLL loaded through a custom injector
- Chain-loading a native payload DLL from the bootstrap
- In-process logging from both bootstrap and payload
- Static analysis of `FM26PlayerExport.dll` to recover the export flow

## Key findings

- The Xbox/MS Store build launches from `C:\Program Files\WindowsApps\...\fm.exe`
- The process loads `C:\Windows\System32\WINHTTP.dll`, not a local proxy DLL from the game folder
- That is why standard BepInEx `winhttp.dll` bootstrap never initializes on this build
- A custom external injector plus native bootstrap works against the running process
- `FM26PlayerExport` is simple at startup: it only attaches `ExportBehaviour`
- The real logic scans UI Toolkit `UIDocument` trees, reads the player table, scrolls it, and writes CSV/HTML under Documents

## Repo layout

- `src/injector/injector.cpp`: external `LoadLibraryW` injector
- `src/probe/bootstrap.cpp`: native bootstrap DLL loaded inside FM26
- `src/payload/payload.cpp`: chain-loaded payload used for in-process diagnostics
- `bin/`: current built binaries used in the working POC
- `analysis/fm26_meta_reader/`: metadata reader used to enumerate types/methods from the managed plugin
- `analysis/fm26_il_dump/`: Mono.Cecil-based IL dumper and dumped `ExportBehaviour` IL
- `build.ps1`: build script for the native POC pieces

## Current next step

Reimplement the essential parts of `FM26PlayerExport.ExportBehaviour` against the live FM26 UI Toolkit tree from the custom payload path, instead of trying to run the original BepInEx plugin unchanged.
