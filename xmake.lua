add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_repositories("nazara-repo https://github.com/NazaraEngine/xmake-repo")

--add_requires("imgui", {configs = {vulkan = true, sdl2 = true, debug = is_mode("debug"), with_symbols = true}})
add_requires("volk", {configs = {header_only = true}})
add_requires("nzsl", {configs = {shared = false}})
add_requires("concerto-core", { debug = true, configs = { asserts = true, shared = true }})
add_requires("vulkan-headers", "vulkan-memory-allocator", "stb", "vulkan-utility-libraries", "parallel-hashmap", "tinyobjloader")
add_requires("libsdl2", {configs = {wayland = is_plat("linux", "bsd"), x11 = is_plat("linux", "bsd")}})

option("override_runtime", { description = "Override vs runtime to MD in release and MDd in debug", default = false })
option("examples", { description = "Build examples", default = false })
option("profiling", { description = "Build with tracy profiler", default = false })
option("object_debug", { description = "Build with graphics object debugging", default = is_mode("debug") })

if is_plat("linux", "bsd") then
    add_defines("CCT_GFX_XLIB")
    add_defines("CCT_GFX_WAYLAND")
end


add_defines("CCT_ENABLE_ASSERTS")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

if has_config("object_debug") then
    add_defines("CCT_ENABLE_OBJECT_DEBUG")
    add_requires("cpptrace")
end

if has_config("profiling") then
    add_requires("tracy[shared]", {configs = {tracy_enable = true, cmake = false}})

    target("concerto-profiler")
        set_kind("shared")
        add_includedirs("Src/", { public = true })
        add_headerfiles("Src/(Concerto/Profiler/*.hpp)")
        add_packages("tracy", {public = true})
        add_packages("concerto-core", {public = false})
        add_defines("CCT_GFX_PROFILING", {public = true})
        add_files("Src/Concerto/Profiler/**.cpp")
        add_rpathdirs("$ORIGIN")
    target_end()
end

if has_config("examples") then
    --add_rules("download.assets")
end

function add_files_to_target(p)
    for _, dir in ipairs(os.filedirs(p)) do
        relative_dir = path.relative(dir, "Src/")
        --print(dir)
        if os.isdir(dir) then
            add_files(path.join("Src", relative_dir, "*.cpp"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.hpp)"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.inl)"))
        else
            local ext = path.extension(relative_dir)
            if ext == ".hpp" or ext == ".inl" then
                add_headerfiles(path.join("Src", "(" .. relative_dir .. ")"))
            elseif ext == ".cpp" then
                add_files(path.join("Src", relative_dir))
            end
        end
    end
end

target("concerto-graphics-core")
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    if is_mode("debug") then
        set_symbols("debug")
    end
    add_defines("CONCERTO_GRAPHICS_CORE_BUILD", { public = false })
    
    add_files_to_target("Src/Concerto/Graphics/Core/Camera/*")
    add_files_to_target("Src/Concerto/Graphics/Core/DisplayManager/*")
    add_files_to_target("Src/Concerto/Graphics/Core/Window/*")
    add_files_to_target("Src/Concerto/Graphics/Core/Primitives/*")
    add_files_to_target("Src/Concerto/Graphics/Core/Input/*")
    --add_files_to_target("Src/Concerto/Graphics/Core/ImGui/*")
    add_files("Src/Concerto/Graphics/Core/*.cpp")
    add_files_to_target("Src/Concerto/Graphics/Core/*.hpp")
    add_rpathdirs("$ORIGIN")

    add_includedirs("Src/", { public = true })
    add_packages("concerto-core", "libsdl2", "vulkan-headers", "nzsl", "imgui", { public = true })
    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end

target("concerto-vulkan-backend", function()
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    
    if is_mode("debug") then
        set_symbols("debug")
    end

    add_defines("CONCERTO_GRAPHICS_VULKAN_BACKEND_BUILD", { public = false })
    add_defines("VK_NO_PROTOTYPES", { public = true })
    add_files("Src/Concerto/Graphics/Backend/Vulkan/*.cpp")

    add_files_to_target("Src/Concerto/Graphics/Backend/Vulkan/*")
    add_files_to_target("Src/Concerto/Graphics/Backend/Vulkan/Wrapper/*")
    add_headerfiles("Src/(Concerto/Graphics/Backend/Vulkan/*.hpp)")

    add_includedirs("Src/", { public = true })
    add_packages("concerto-core", "volk", "vulkan-headers", "vulkan-utility-libraries", "vulkan-memory-allocator", "nzsl", { public = true })
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
        
        add_files("Src/Concerto/Graphics/Backend/Dx12/*.cpp")
        add_files_to_target("Src/Concerto/Graphics/Backend/Dx12/*")
        add_files_to_target("Src/Concerto/Graphics/Backend/Dx12/Wrapper/*")
        add_headerfiles("Src/(Concerto/Graphics/Backend/Dx12/*.hpp)")

        add_includedirs("Src/", { public = true })
        add_headerfiles("Src/(Concerto/Graphics/Backend/Dx12/*.hpp)")
        add_packages("concerto-core", "nzsl", { public = true })
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

target("concerto-rhi-module")
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("allextra")
    if is_mode("debug") then
        set_symbols("debug")
    end
    add_packages("stb", { public = false })
    add_defines("CONCERTO_GRAPHICS_RHI_MODULE_BUILD", { public = false })

    add_files_to_target("Src/Concerto/Graphics/RHI/*")
    add_files_to_target("Src/Concerto/Graphics/RHI/Vulkan/*")

    if is_plat("windows") then
        add_files_to_target("Src/Concerto/Graphics/RHI/Dx12/*")
        add_deps("concerto-dx12-backend")
        add_syslinks("d3d12")
        add_defines("NZSL_WITH_DXC")
    end
    add_deps("concerto-vulkan-backend")

    add_packages("concerto-core", "parallel-hashmap", { public = true })
    add_packages("nazaraengine", "tinyobjloader", { public = true })
    add_rpathdirs("$ORIGIN")

    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end

includes("Xmake/Rules/*.lua")

--if has_config("examples") then
    includes("Examples/xmake.lua")
--end
