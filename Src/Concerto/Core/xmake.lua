add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

option("unitybuild", { description = "Build using unity build", default = false })
option("tests", { description = "Enable unit tests", default = false})
option("examples", { description = "Enable examples", default = false})
option("static", { description = "Build static library", default = false })
option("override_runtime", { description = "Override vs runtime to MD in release and MDd in debug", default = true })
option("asserts", { description = "Enable asserts", default = false })
option("enet", { description = "Enable ENet support", default = true })

if is_plat("windows") and has_config("override_runtime") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

if has_config("enet") then
    add_requires("enet", {configs = {shared = false}})
end

add_requires("spdlog")

target("concerto-core", function()
    set_kind("$(kind)")
    set_warnings("allextra")
    set_languages("cxx20")

    if is_kind("static") then
        add_defines("CCT_CORE_LIB_STATIC", {public = true})
    end

    if is_mode("debug") then
        set_symbols("debug")
    end

    if has_config("enet") then
        add_packages("enet", {public = true})
        add_defines("CCT_ENABLE_ENET")
    end

    add_packages("spdlog")
    add_defines("CCT_CORE_BUILD")
    add_cxxflags("cl::/Zc:preprocessor", { public = true })
    add_cxxflags("cl::/utf-8")
    add_includedirs("../../", {public = true})
    
    local files = {
        "Any",
        "Buffer",
        "Cast",
        "DeferredExit",
        "DynLib",
        "EnumFlags",
        "Error",
        "FunctionRef",
        "Logger",
        "Math",
        "Network",
        "Result",
        "Serializer",
        "SparseVector",
        "Stream",
        "ThreadPool",
        "TypeInfo",
        "Types",
    }

    for _, file in ipairs(files) do
        add_files_to_target("./" .. file, false, true)
    end

    add_cxxflags("cl::/wd4251")

    if is_plat("windows", "mingw") then
        add_syslinks("ws2_32", "Kernel32")
    end

    if is_plat("linux") then
        add_syslinks("dl")
    end

    if has_config("unitybuild") then
        add_rules("c++.unity_build", {batchsize = 12, uniqueid = "CCT_UNITY_BUILD_ID"})
    end

    if has_config("asserts") then
        add_defines("CCT_ENABLE_ASSERTS")
    end

    -- macOS: ensure we link against the correct C++ runtime when using custom toolchain
    if is_plat("macosx") then
        local llvm_prefix = os.getenv("LLVM_PREFIX")
        if llvm_prefix then
            add_linkdirs(path.join(llvm_prefix, "lib"))
            add_rpathdirs(path.join(llvm_prefix, "lib"))
        end
    end
end)
