target('concerto-rendergraph-sample')
    add_deps('concerto-rhi-module')
    add_files('main.cpp')
    set_languages('cxx20')
    set_warnings('allextra')
    add_deps("concerto-core", { public = true })
    add_rpathdirs("$ORIGIN")

    if has_config("profiling") then
        add_deps("concerto-profiler", { public = false })
    end
