# Generated Visual Studio project files

This folder contains the `DarkEdenLegacyModern.sln` solution generated from the Phoenix Client CMake project, plus the `.vcxproj` files referenced by that solution and their optional `.filters` files.

These are shared as reference files only, copied from one developer's local `F:\Project Phoenix\build` directory. The generated projects contain absolute paths to that computer's source tree and vcpkg installation. They will not build on another computer as-is.

For a portable build, use the actual source and CMake instructions in [`../README.md`](../README.md). Configure a fresh build directory on your computer; CMake will generate a solution with local paths. The current source also lacks the legacy DirectX 7 SDK files, which may be required to complete compilation.

The generated project set intentionally excludes `.vcxproj.user`, CMake caches, object files, binaries, logs, and other machine-specific build output.
