# Building the Client with Visual Studio

This guide explains how to build the DK Project Phoenix Client source on another Windows computer using Visual Studio 2022 or later.

> **Important:** The `.sln`, `.vcxproj`, CMake cache files, and intermediate outputs under `F:\Project Phoenix\build` contain paths tied to the original development machine. They are not portable project inputs. The source code and portable CMake/vcpkg configuration are in the [`Client/`](https://github.com/dkcommunity/DK-Project-Phoenix/tree/main/Client) directory of the main repository. Generate a fresh build directory on each computer.

## Requirements

- Windows 10 or 11
- Visual Studio 2022 or later with the **Desktop development with C++** workload
- MSVC C++ build tools and the Windows 10/11 SDK
- CMake 3.20 or later
- Git
- vcpkg

The project targets Win32/x86 and installs its dependencies from the `Client/vcpkg.json` manifest.

## Prepare the source and dependencies

Clone the source repository in PowerShell:

```powershell
git clone https://github.com/dkcommunity/DK-Project-Phoenix.git C:\src\DK-Project-Phoenix
Set-Location C:\src\DK-Project-Phoenix\Client
$clientDir = (Get-Location).Path
```

If vcpkg is not already installed, install it and configure it for the current PowerShell session:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\src\vcpkg
Set-Location C:\src\vcpkg
.\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = 'C:\src\vcpkg'
Set-Location $clientDir
```

If vcpkg is already installed, set `VCPKG_ROOT` to its existing installation directory instead.

Clone the SoLoud source at the version expected by the project:

```powershell
git clone https://github.com/jarikomppa/soloud.git .\thirdparty\soloud
git -C .\thirdparty\soloud checkout RELEASE_20200207
```

## Generate the Visual Studio solution and build

The following example generates a Win32 solution for Visual Studio 2022. During configuration, CMake uses the vcpkg manifest to install the declared packages:

```powershell
cmake -S . -B build `
  -G "Visual Studio 17 2022" `
  -A Win32 `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET=x86-windows

cmake --build build --config Release --target Client --parallel
```

In Visual Studio, open the `Client` folder with **Open > Folder** to use it as a CMake project, or open the generated `build\DarkEdenLegacyModern.sln` solution. For newer Visual Studio releases, select the CMake generator supported by your installed version, such as the corresponding `Visual Studio <version> <year>` generator.

The executable and runtime DLLs are written to `Client\GameData` according to the CMake configuration.

## Known missing dependency: DirectX 7 SDK

The source tree does not include `Client\thirdparty\dx7sdk`. CMake warns when this SDK is missing, but source files that require legacy DirectX 7 headers or libraries may fail to compile. The SDK is not included in this repository; a licensed, compatible copy may need to be obtained separately and placed in the expected location.

Therefore, these steps provide a portable CMake/vcpkg setup, but a successful clean build is not guaranteed until the required DirectX 7 files are available.

## Files not included from the `build` directory

The CMake cache, generated Visual Studio solution/project files, `.vs`, `CMakeFiles`, `vcpkg_installed`, object files, build logs, and previously compiled binaries under `F:\Project Phoenix\build` are tied to the local machine or a particular build configuration. Do not use them as build inputs on another computer; regenerate them from the source on each machine.
