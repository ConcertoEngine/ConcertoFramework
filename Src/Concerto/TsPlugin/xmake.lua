target("concerto-ts-plugin", function()
    set_kind("shared")
    set_languages("c11")
    add_rpathdirs("$ORIGIN")
    add_files("*.c")
    add_includedirs("../../../Src", { public = true })
    add_deps("concerto-plugin-api")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if has_config("profiling") then
        if is_plat("windows") then
            add_deps("concerto-profiler-mt", { public = false })
        else
            add_deps("concerto-profiler", { public = false })
        end
    end
end)
