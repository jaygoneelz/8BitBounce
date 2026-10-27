# Native desktop setup

The desktop project targets Windows x64, C++17 and OpenGL. Retained entry points are `engine.sln`, `engine/engine.vcxproj`, `game/game.vcxproj` and `premake5.lua`. The solution also contains build projects for GLFW, GLAD, Bullet, FreeType and stb_image.

## Prerequisites

Install Visual Studio's Desktop development with C++ workload, a Windows SDK and the MSVC v143 toolset. Use Debug / x64 first. The project expects dependency folders under `engine/external/`.

The following are local prerequisites rather than publicly redistributed SDK binaries:

| Dependency | Expected layout |
| --- | --- |
| FMOD Core API | `engine/external/FMOD/include/fmod.hpp`, related Core headers, `lib/x64/fmod_vc.lib` and `lib/x64/fmod.dll` |
| Assimp | Headers under `engine/external/assimp/include/assimp/`; compatible x64 Debug import/static library `assimp-vc143-mtd.lib` in the library path configured by `engine.vcxproj` |
| GLFW, GLAD, stb_image, Bullet, FreeType | Source/projects are retained; matching compiled libraries must be available in the configured dependency library paths |
| Arial-compatible font | Supply a font you are permitted to use at `game/assets/fonts/arial.ttf`, or change the font path in `text_manager.cpp` |

Get FMOD through its official distribution and comply with its license. SDK headers and import libraries are not bundled in the public repository. The existing native working copy retains these local prerequisites, so preparation does not destroy them.

## Build and launch

From a Visual Studio Developer PowerShell:

```powershell
msbuild engine.sln /p:Configuration=Debug /p:Platform=x64 /m
```

Set `game` as the startup project and `game/` as its working directory so relative `assets/` paths resolve. The executable is produced under `bin/Debug-windows-x86_64/game/game.exe`. Keep the required runtime DLLs beside the executable.

The retained Premake script and older generator batch files are optional reference files. A Premake executable is not redistributed. The checked-in solution can be opened directly.

## Verification status

The native Debug/x64 solution built successfully on 27 October 2026 with the local dependencies and Visual Studio's installed v143 tools: **0 errors, 460 warnings**. The warnings include older dependency deprecations, numeric conversions and missing third-party debug symbols. This is a compilation/link check, not a claim that every runtime feature or configuration has been tested.

A source-only fresh clone was not verified without restoring these dependencies. Release and Dist configurations have different library paths/names and have not been validated. The browser game is independently runnable and does not depend on any native SDK.
