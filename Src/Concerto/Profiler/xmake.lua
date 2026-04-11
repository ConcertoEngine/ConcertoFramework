if has_config("profiling") then
    add_requires("tracy[shared]", {configs = {tracy_enable = true, cmake = false}})

    target("concerto-profiler", function ()         
        set_kind("shared")
        add_includedirs("Src/", { public = true })
        add_headerfiles("Src/(Concerto/Profiler/*.hpp)")
        add_packages("tracy", {public = true})
        add_packages("concerto-core", {public = false})
        add_defines("CCT_GFX_PROFILING", {public = true})
        add_files("Src/Concerto/Profiler/**.cpp")
        add_rpathdirs("$ORIGIN")
    end)
end