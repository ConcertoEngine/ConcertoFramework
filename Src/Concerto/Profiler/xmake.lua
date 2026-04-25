if has_config("profiling") then
    add_requires("tracy", {configs = {shared = true, cmake = false, debug = true, vs_runtime = is_mode("debug") and "MDd" or "MD"}})
end
    
target("concerto-profiler", function()
    set_kind("shared")
    add_includedirs("../../../Src", { public = true })
    add_headerfiles("../../../Src/(Concerto/Profiler/*.hpp)")
    if has_config("profiling") then
        add_packages("tracy", {public = true})
        add_defines("CCT_PROFILING", {public = true})
    end
    add_files("./**.cpp")
    add_rpathdirs("$ORIGIN")
    add_deps("concerto-core", { public = true })
end)

if is_plat("windows") then
    target("concerto-profiler-mt", function()
        set_kind("shared")
        add_includedirs("../../../Src", { public = true })
        add_headerfiles("*.hpp")
        if has_config("profiling") then
            add_packages("tracy", {public = true})
            add_defines("CCT_PROFILING", {public = true})
        end
        add_files("./**.cpp")
        add_rpathdirs("$ORIGIN")
        if is_plat("windows") then
            add_deps("concerto-core-mt", { public = true })
            set_runtimes("MT")
        else
            add_deps("concerto-core", { public = true })
        end
    end)
end