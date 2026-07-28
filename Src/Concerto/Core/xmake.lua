option("unitybuild", { description = "Build using unity build", default = false })
option("tests", { description = "Enable unit tests", default = false})
option("examples", { description = "Enable examples", default = false})
option("static", { description = "Build static library", default = false })
option("asserts", { description = "Enable asserts", default = false })
option("enet", { description = "Enable ENet support", default = true })

add_requires("stduuid")

if is_plat("windows") then
    add_requires("enet6",     {configs = {shared = false,      runtimes = is_mode("debug") and "MDd" or "MD"}})
    add_requires("spdlog",    {configs = {header_only = false, runtimes = is_mode("debug") and "MDd" or "MD"}})

    if has_config("reflection") then
        add_requires("enet6~mt",  {alias = "enet6-mt",  configs = {shared = false,      runtimes = "MT"}})
        add_requires("spdlog~mt", {alias = "spdlog-mt", configs = {header_only = false, runtimes = "MT"}})
    end
else
    add_requires("enet6",  {configs = {shared = false}})
    add_requires("spdlog", {configs = {header_only = false}})
end

local concerto_core = {
    concerto_core_mt = {
        enabled = is_plat("windows") and has_config("reflection"),
        kind = get_config("kind") or "shared",
        runtimes = "MT",
        packages = {"spdlog-mt"},
        configs = {
            enet = {
                packages = {"enet6-mt"},
                defines = {"CCT_ENABLE_ENET"}
            }
        }
    },
    concerto_core = {
        enabled = true,
        kind = get_config("kind") or "shared",
        runtimes = is_mode("debug") and "MDd" or "MD",
        packages = {"spdlog"},
        configs = {
            enet = {
                packages = {"enet6"},
                defines = {"CCT_ENABLE_ENET"}
            }
        }
    },
}

for targetName, targetConfig in pairs(concerto_core) do
    if not targetConfig.enabled then
        goto continue
    end
    target(targetName:gsub("_", "-"))
        set_kind(targetConfig.kind)
        set_warnings("allextra")
        set_languages("cxx20")

        if is_plat("windows") then
            set_runtimes(targetConfig.runtimes)
        end

        if is_kind("static") then
            add_defines("CCT_CORE_LIB_STATIC", {public = true})
        end

        if is_mode("debug") then
            set_symbols("debug")
        end

        for _, pkg in ipairs(targetConfig.packages) do
            add_packages(pkg, {public = true})
        end

        add_packages("stduuid")

        for configName, config in pairs(targetConfig.configs) do
            if has_config(configName) then
                for _, define in ipairs(config.defines or {}) do
                    add_defines(define, {public = true})
                end
                for _, pkg in ipairs(config.packages or {}) do
                    add_packages(pkg, {public = true})
                end
            end
        end

        add_defines("CCT_CORE_BUILD")
        add_cxxflags("cl::/Zc:preprocessor", { public = true })
        add_cxxflags("cl::/utf-8")
        add_includedirs("../../", {public = true})

        if is_plat("mingw", "linux", "macosx", "bsd") then
            add_syslinks("pthread")
        end

        if is_plat("linux", "macosx", "bsd") then
            add_cxxflags("-fPIC", {force = true})
        end

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
            "Network/ENet",
            "Network/ENet/ENet",
            "Network/ENet/Address",
            "Network/ENet/Client",
            "Network/ENet/ENet",
            "Network/ENet/Host",
            "Network/ENet/Packet",
            "Network/ENet/Peer",
            "Network/ENet/Server",
            "Network/IpAddress",
            "Network/Packet",
            "Network/Socket",
            "Result",
            "Serializer",
            "Signal",
            "SparseVector",
            "Stream",
            "Thread",
            "ThreadPool",
            "ThreadQueue",
            "TypeInfo",
            "Types",
            "Uuid",
        }

        for _, file in ipairs(files) do
            add_files_to_target("./" .. file, false, true)
        end

        add_cxxflags("cl::/wd4251")

        if is_plat("windows", "mingw") then
            add_syslinks("ws2_32", "Kernel32", "Ole32")
        end

        if is_plat("linux") then
            add_syslinks("dl")
        end

        if has_config("unitybuild") then
            add_rules("c++.unity_build", {batchsize = 12, uniqueid = "CCT_UNITY_BUILD_ID"})
        end

        if has_config("asserts") then
            add_defines("CCT_ENABLE_ASSERTS", { public = true })
        end

        -- macOS: ensure we link against the correct C++ runtime when using custom toolchain
        if is_plat("macosx") then
            local llvm_prefix = os.getenv("LLVM_PREFIX")
            if llvm_prefix then
                add_linkdirs(path.join(llvm_prefix, "lib"))
                add_rpathdirs(path.join(llvm_prefix, "lib"))
            end
        end
    target_end()
    ::continue::
end