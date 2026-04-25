# Concerto Reflection Plugin System

## Overview

The Concerto Reflection plugin system allows you to extend the reflection code generator capabilities in a modular way. It provides a flexible architecture based on a stable C API to ensure ABI compatibility across different compilers and versions.

## Architecture

### Core Components

1. **PluginApi.h/cpp** - Pure C API for cross-compiler compatibility
2. **PluginRegistry** - Plugin manager with dynamic loading
3. **ReflectionGeneratorPlugin** - Optional C++ interface to simplify development
4. **DefaultGeneratorsPlugin** - Built-in plugin providing Header and Cpp generators

### Flow Diagram

```
PackageGenerator (main)
    │
    ├─> PluginRegistry
    │       ├─> LoadPlugin(path)
    │       ├─> TransformPackage()
    │       └─> CollectGenerators()
    │
    └─> Plugins (DLL/SO)
            ├─> DefaultGeneratorsPlugin (built-in)
            └─> Custom Plugins (user-defined)
```

## Plugin API

### Basic Structure

Each plugin must export a function table via a standard entry point:

```c
CRP_PLUGIN_EXPORT const CrpPluginFunctionTable* crpGetPluginFunctionTable(void);
```

### Function Table

```c
typedef struct CrpPluginFunctionTable {
    uint32_t apiVersion;                    // API version (required)

    // Metadata
    void (*GetInfo)(CrpPluginInfo* outInfo);  // Required
    int32_t (*GetPriority)(void);             // Optional (default: 0)

    // Lifecycle
    int32_t (*OnLoad)(void);                  // Optional
    void (*OnUnload)(void);                   // Optional

    // Transformation
    void (*TransformPackage)(CrpPackage* package);  // Optional

    // File generators
    int32_t (*CreateGenerators)(...);         // Optional
    void (*DestroyGenerator)(...);            // Optional

    // Generation hooks (all optional)
    void (*BeforePackageGeneration)(CrpGenerationContext* ctx);
    void (*AfterPackageGeneration)(CrpGenerationContext* ctx);
    void (*BeforeNamespaceGeneration)(...);
    void (*AfterNamespaceGeneration)(...);
    void (*BeforeClassGeneration)(...);
    void (*AfterClassGeneration)(...);
    void (*BeforeMemberGeneration)(...);
    void (*AfterMemberGeneration)(...);
    void (*BeforeMethodGeneration)(...);
    void (*AfterMethodGeneration)(...);
    void (*BeforeEnumGeneration)(...);
    void (*AfterEnumGeneration)(...);
    void (*BeforeTemplateClassGeneration)(...);
    void (*AfterTemplateClassGeneration)(...);
    void (*BeforeGenericClassGeneration)(...);
    void (*AfterGenericClassGeneration)(...);

    // Custom annotations
    int32_t (*HandleAnnotation)(...);         // Optional
} CrpPluginFunctionTable;
```

## Priority System

Plugins are executed in **descending** priority order:
- High priority (e.g., 100) → executed first
- Default priority (0)
- Low priority (e.g., -100) → executed last

The `DefaultGeneratorsPlugin` uses a priority of **-100** to execute first, allowing other plugins to add hooks after the base generators are created.

## Generation Hooks

Hooks allow you to inject code at different stages of the generation process:

### Execution Order

```
BeforePackageGeneration
    └─> BeforeNamespaceGeneration
            ├─> BeforeClassGeneration
            │       ├─> BeforeMemberGeneration
            │       ├─> AfterMemberGeneration
            │       ├─> BeforeMethodGeneration
            │       └─> AfterMethodGeneration
            └─> AfterClassGeneration
            ├─> BeforeEnumGeneration
            └─> AfterEnumGeneration
        AfterNamespaceGeneration
AfterPackageGeneration
```

### GenerationContext

The context provided to hooks allows writing to the output stream:

```cpp
struct GenerationContext {
    const Package* package;
    const Namespace* currentNamespace;
    const Class* currentClass;
    const Enum* currentEnum;
    std::ostringstream* outputStream;
    std::size_t* indentLevel;
    std::string_view apiMacro;

    // Utility methods
    void Write(format_string, ...);
    void NewLine();
    void EnterScope();
    void LeaveScope(suffix = "");
};
```

## C API Accessors

The API provides accessors for all opaque types:

### Package
- `crpPackageGetName()`
- `crpPackageGetVersion()`
- `crpPackageGetClassCount()` / `crpPackageGetClass()`
- `crpPackageGetNamespaceCount()` / `crpPackageGetNamespace()`
- `crpPackageGetEnumCount()` / `crpPackageGetEnum()`

### Class
- `crpClassGetName()` / `crpClassGetBase()` / `crpClassGetScope()`
- `crpClassGetMethodCount()` / `crpClassGetMethod()`
- `crpClassGetMemberCount()` / `crpClassGetMember()`
- `crpClassIsTemplate()` / `crpClassIsGeneric()`

### Enum
- `crpEnumGetName()` / `crpEnumGetBase()`
- `crpEnumGetElementCount()`

### Namespace
- `crpNamespaceGetName()`
- `crpNamespaceGetClassCount()` / `crpNamespaceGetClass()`
- `crpNamespaceGetEnumCount()` / `crpNamespaceGetEnum()`

### ClassMember
- `crpClassMemberGetName()` / `crpClassMemberGetType()`
- `crpClassMemberIsNative()`

### ClassMethod
- `crpClassMethodGetName()` / `crpClassMethodGetBase()`
- `crpClassMethodGetReturnType()`
- `crpClassMethodHasCustomInvoker()`

### GenerationContext Utilities (C API)
These functions allow pure C plugins to write to the generation context:

- `crpGenerationContextWrite(ctx, format, ...)` - Write formatted text with automatic indentation
- `crpGenerationContextNewLine(ctx)` - Add a blank line
- `crpGenerationContextEnterScope(ctx)` - Open a scope `{` and increase indentation
- `crpGenerationContextLeaveScope(ctx, suffix)` - Close a scope `}` with optional suffix (e.g., `;`) and decrease indentation

**Example:**
```c
crpGenerationContextWrite(ctx, "void MyFunction()");
crpGenerationContextEnterScope(ctx);
crpGenerationContextWrite(ctx, "printf(\"Hello\\n\");");
crpGenerationContextLeaveScope(ctx, NULL);
```

All accessors check for NULL pointers and return safe values (`nullptr`, `""`, or `0`).

## Loading Plugins

### Command Line

```bash
concerto-pkg-generator output-dir \
    -P path/to/plugin1.dll \
    -P path/to/plugin2.so \
    --plugin-dir path/to/plugins/directory
```

### Xmake Integration

The system automatically detects plugins in dependencies:

```lua
target("my-custom-plugin")
    set_kind("shared")
    add_deps("concerto-plugin-api")
    -- Name must contain "generator-plugin", "generators-plugin" or "generators"

target("my-library")
    add_deps("concerto-reflection", "my-custom-plugin")
    add_rules("cct_cpp_reflect")
```

The `cct_cpp_reflect` rule automatically collects plugins from dependencies and passes them to the generator.

## Validation and Security

The `PluginRegistry` performs several validations during loading:

1. **API Version**: Checks that `apiVersion == CRP_PLUGIN_API_VERSION`
2. **Entry Point**: Verifies the presence of `crpGetPluginFunctionTable`
3. **Required GetInfo**: The `GetInfo` function must be provided
4. **OnLoad**: If present, must return `true` for success

Loading failure → plugin is rejected with an error message in logs.

## Plugin Lifecycle

1. **Loading**: `DynLib::Load()` loads the library
2. **Resolution**: Retrieval of entry point `crpGetPluginFunctionTable`
3. **Validation**: API version and required functions check
4. **Initialization**: Call to `OnLoad()` if present
5. **Registration**: Added to registry and sorted by priority
6. **Usage**: Hooks and generators called during generation
7. **Unloading**: Call to `OnUnload()` if present
8. **Release**: DLL unloaded via `DynLib`

## Use Cases

### Transformation Plugin
Modify the package before generation (rename classes, add metadata, etc.)

### Code Generation Plugin
Create additional files (bindings, serialization, etc.)

### Hooks Plugin
Inject code into generated files (logging, validation, etc.)

### Annotations Plugin
Process custom annotations ([@serialize], [@network], etc.)

## See Also

- [Plugin Creation Guide](CreatingPlugins.md)
- [Plugin Examples](PluginExamples.md)
- [API Reference](PluginApiReference.md)
