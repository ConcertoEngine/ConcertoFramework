add_repositories("nazara-repo https://github.com/NazaraEngine/xmake-repo")

add_requires("volk", {configs = {header_only = true}})
add_requires("nzsl", {configs = {shared = false}})
add_requires("vulkan-headers", "vulkan-memory-allocator", "stb", "vulkan-utility-libraries", "parallel-hashmap", "tinyobjloader")
add_requires("libsdl3", {configs = {wayland = is_plat("linux", "bsd"), x11 = is_plat("linux", "bsd")}})

option("examples", { description = "Build examples", default = false })
option("object_debug", { description = "Build with graphics object debugging", default = is_mode("debug") })

if is_plat("linux", "bsd") then
    add_defines("CCT_GFX_XLIB")
    add_defines("CCT_GFX_WAYLAND")
end


if has_config("object_debug") then
    add_defines("CCT_ENABLE_OBJECT_DEBUG")
    add_requires("cpptrace")
end

target("concerto-graphics-core", function()
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    if is_mode("debug") then
        set_symbols("debug")
    end
    add_defines("CONCERTO_GRAPHICS_CORE_BUILD", { public = false })
    add_includedirs("../../", { public = true })
    
    local files = {
        "Camera",
        "DisplayManager",
        "Window",
        "Primitives",
        "ShaderModule",
        "ShaderModuleLoader",
        "Input",
    }

    for _, file in ipairs(files) do
        add_files_to_target("./Core/" .. file .. "/*")
    end
    add_files_to_target("./Core/*.hpp")
    add_deps("concerto-core")
    add_packages("libsdl3", "vulkan-headers", "nzsl", "imgui", { public = true })
    add_rpathdirs("$ORIGIN")
    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end
end)

target("concerto-vulkan-backend", function()
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    
    if is_mode("debug") then
        set_symbols("debug")
    end

    add_defines("CONCERTO_GRAPHICS_VULKAN_BACKEND_BUILD", { public = false })
    add_defines("VK_NO_PROTOTYPES", { public = true })

    add_files_to_target("./Backend/Vulkan/*")
    add_files_to_target("./Backend/Vulkan/Wrapper/*")
    add_headerfiles("../../(Concerto/Graphics/Backend/Vulkan/*.hpp)")

    add_includedirs("../../", { public = true })
    add_deps("concerto-core", { public = false })
    add_packages("volk", "vulkan-headers", "vulkan-utility-libraries", "vulkan-memory-allocator", "nzsl", { public = true })
    add_deps("concerto-graphics-core")
    add_rpathdirs("$ORIGIN")

    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end

    if has_config("object_debug") then
        add_packages("cpptrace", { public = false })
    end
end)

if is_plat("windows") then
    target("concerto-dx12-backend", function()
        set_kind("shared")
        set_languages("cxx20")
        set_warnings("allextra")
        
        if is_mode("debug") then
            set_symbols("debug")
        end

        add_defines("CONCERTO_GRAPHICS_DX12_BACKEND_BUILD", { public = false })

        add_files_to_target("./Backend/Dx12/*")
        add_files_to_target("./Backend/Dx12/Wrapper/*")
        add_headerfiles("../../(Concerto/Graphics/Backend/Dx12/*.hpp)")

        add_includedirs("../../", { public = true })
        add_headerfiles("../../(Concerto/Graphics/Backend/Dx12/*.hpp)")
        add_deps("concerto-core")
        add_packages("nzsl", { public = true })
        add_deps("concerto-graphics-core")
        add_rpathdirs("$ORIGIN")
        add_syslinks("d3d12", "dxgi", "dxguid")

        if has_config("profiling") then
            add_deps("concerto-profiler", { public = false })
        end

        if has_config("object_debug") then
            add_packages("cpptrace", { public = false })
        end
    end)
end

target("concerto-rhi-module", function()
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    if is_mode("debug") then
        set_symbols("debug")
    end
    add_packages("stb", { public = false })
    add_defines("CONCERTO_GRAPHICS_RHI_MODULE_BUILD", { public = false })

    add_files_to_target("./RHI/*")
    add_files_to_target("./RHI/Vulkan/*")
    add_files_to_target("./RenderGraph/*")

    if is_plat("windows") then
        add_files_to_target("./RHI/Dx12/*")
        add_deps("concerto-dx12-backend")
        add_syslinks("d3d12")
        add_defines("NZSL_WITH_DXC")
    end
    add_deps("concerto-vulkan-backend")

    add_deps("concerto-core")
    add_packages("parallel-hashmap", { public = true })
    add_packages("nazaraengine", "tinyobjloader", { public = true })
    add_rpathdirs("$ORIGIN")

    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end
end)
