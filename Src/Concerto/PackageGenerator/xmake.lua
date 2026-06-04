target("concerto-plugin-api", function()
    set_kind("shared")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_files("./Plugin/PluginApi.cpp")
    add_headerfiles("../../../Src/(Concerto/PackageGenerator/Plugin/PluginApi.h)")
    add_headerfiles("../../../Src/(Concerto/PackageGenerator/Defines.hpp)")
    add_includedirs("../../../Src", { public = true })
    if is_plat("windows") then
        add_deps("concerto-core-mt", { public = false })
    else
        add_deps("concerto-core", { public = false })
    end
    add_packages("toml11", { public = true })
    add_defines("CRP_PLUGIN_API_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end
    if has_config("profiling") then
        if is_plat("windows") then
            add_deps("concerto-profiler-mt", { public = true })
        else
            add_deps("concerto-profiler", { public = true })
        end
    end
end)

target("concerto-pkg-generator", function()
    set_kind("binary")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    local files = { ".", "ClangParser", "Plugin" }
    for _, dir in ipairs(files) do
        add_files_to_target("./" .. dir, false)
    end
    remove_files("./Plugin/PluginApi.cpp")
    add_includedirs("../../../Src", { public = true })

    if is_plat("windows") then
        add_deps("concerto-core-mt", { public = false })
    else
        add_deps("concerto-core", { public = true })
    end
    
    add_packages("toml11", { public = true })
    add_packages("libllvm", "cxxopts")
    add_deps("concerto-plugin-api")
    -- LLVM 21 marks all Clang classes with CLANG_ABI (__declspec(dllimport) by default on Windows).
    -- We link against static clang libs, so suppress the dllimport decoration.
    add_defines("CLANG_BUILD_STATIC")
    set_policy("build.fence", true)
    add_defines("CCT_PKGGENERATOR_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end

    on_config(function(package)
        import("core.project.project")
        local llvm = project.required_package("libllvm")
        assert(llvm, "libllvm not found!")
        local llvm_lib = path.join(llvm:installdir(), "lib")
        package:add("rpathdirs", llvm_lib)
    end)
end)