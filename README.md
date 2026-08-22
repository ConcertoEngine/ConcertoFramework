# ConcertoFramework

C++ framework combining the **Core**, **Graphics** and **Reflection** modules of the Concerto project.  
Developed for learning purposes.

## Build Status

[![Build and test](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/build.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/build.yml)
[![Coverage](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/coverage.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/coverage.yml)

A single workflow covers every platform: Windows x64, Linux x86_64 (also under ASan, LSan and TSan) and macOS x86_64 / arm64, each in `shared` and `static`.

## Modules

### Core (`Src/Concerto/Core`)
Foundation library: types, math, networking (ENet), serialization, signals, thread pool, process spawning, DynLib, logger (spdlog), and more.

### Graphics (`Src/Concerto/Graphics`)
Low-level rendering layer featuring:
- **RHI** - multi-backend abstraction (Vulkan / DX12)
- **Vulkan backend** - Vulkan wrapping via volk + VMA
- **DX12 backend** - Direct3D 12 wrapping (Windows only)
- **Profiler** - optional Tracy integration

Off by default, enable with `--graphics=y`.

### Reflection (`Src/Concerto/Reflection`)
C++ reflection system: classes, enums, namespaces, methods and member variables described at runtime, with JSON and binary codecs on top.

Descriptions are generated ahead of the build by `concerto-pkg-generator`, which parses the annotated headers with clang and drives a set of output plugins (`HeaderPlugin`, `CppPlugin`, `TsPlugin`, `JsonPlugin`) — see [Docs/PluginSystem.md](Docs/PluginSystem.md).

Off by default, enable with `--reflection=y`. It pulls libllvm, which is why CI only builds it on Windows: elsewhere the job times out.

## Requirements

- [xmake](https://xmake.io) >= 2.8
- C++20 compiler (MSVC, Clang, GCC)
- Vulkan SDK (for the Vulkan backend)
- DX12 (Windows only, included in the Windows SDK)

Everything else, libllvm included, is fetched by xmake.

## Build

```bash
# Debug
xmake config -m debug

# Release
xmake config -m release

# With the graphics module and its examples
xmake config -m debug --graphics=y --examples=y

# With the reflection system
xmake config -m debug --reflection=y

# With Tracy profiler
xmake config -m debug --profiling=y
```

```bash
xmake
```

## Tests

```bash
xmake config --tests=y
xmake
xmake run concerto-tests
```

`concerto-tests` is the only runnable test target: `concerto-core-tests` and `concerto-reflection-tests` are object libraries linked into it. The reflection suite is part of the binary only when the build was configured with `--reflection=y`.

## IDE Project Generation

```bash
# CMakeLists
xmake project -k cmakelists

# Visual Studio
xmake project -k vsxmake
```

## Structure

```
Src/
  Concerto/
    Core/             # Core module
    Graphics/         # Graphics module (RHI, Vulkan/DX12 backends)
    Profiler/         # Profiler module (Tracy)
    Reflection/       # Reflection runtime
    PackageGenerator/ # Reflection generator (clang parser + plugin API)
    HeaderPlugin/     # Generator plugin: .gen.hpp
    CppPlugin/        # Generator plugin: .gen.cpp
    TsPlugin/         # Generator plugin: .gen.ts type definitions
    JsonPlugin/       # Generator plugin: .Accept.gen.cpp field visitors
Examples/
  Core/               # Core examples
  Graphics/           # Graphics examples (RHI)
Tests/
  Core/               # Core unit tests (Catch2)
  Reflection/         # Reflection unit tests (Catch2)
Docs/                 # Plugin system documentation
Shaders/              # NZSL / GLSL shaders
Xmake/                # Custom xmake rules
assets/               # Test assets
```
