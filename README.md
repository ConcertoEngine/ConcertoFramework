# ConcertoFramework

C++ framework combining the **Core** and **Graphics** modules of the Concerto project.  
Developed for learning purposes.

## Build Status

| Platform | Status |
|----------|--------|
| Windows  | [![Windows](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/windows.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/windows.yml) |
| Linux    | [![Linux](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/ubuntu24.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/ubuntu24.yml) |
| macOS    | [![macOS](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/macos.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoFramework/actions/workflows/macos.yml) |

## Modules

### Core (`Src/Concerto/Core`)
Foundation library: types, math, networking (ENet), serialization, signals, thread pool, DynLib, logger (spdlog), and more.

### Graphics (`Src/Concerto/Graphics`)
Low-level rendering layer featuring:
- **RHI** - multi-backend abstraction (Vulkan / DX12)
- **Vulkan backend** - Vulkan wrapping via volk + VMA
- **DX12 backend** - Direct3D 12 wrapping (Windows only)
- **Profiler** - optional Tracy integration

## Requirements

- [xmake](https://xmake.io) >= 2.8
- C++20 compiler (MSVC, Clang, GCC)
- Vulkan SDK (for the Vulkan backend)
- DX12 (Windows only, included in the Windows SDK)

## Build

```bash
# Debug
xmake config -m debug

# Release
xmake config -m release

# With examples
xmake config -m debug --examples=true

# With Tracy profiler
xmake config -m debug --profiling=true
```

```bash
xmake
```

## Tests

```bash
xmake config --tests=true
xmake
xmake run concerto-core-tests
```

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
    Core/       # Core module
    Graphics/   # Graphics module (RHI, Vulkan/DX12 backends)
    Profiler/   # Profiler module (Tracy)
Examples/
  Core/         # Core examples
  Graphics/     # Graphics examples (RHI)
Tests/
  Core/         # Core unit tests (Catch2)
Shaders/        # NZSL / GLSL shaders
Xmake/          # Custom xmake rules
assets/         # Test assets
```
