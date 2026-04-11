target("concerto-core-dummy", function()
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("none")
    add_deps("concerto-core")
    set_default(false)
    
    on_config(function(target)
        local dummy_file = path.join(target:autogendir(), "dummy.cpp")
        target:add("files", dummy_file)
        io.writefile(dummy_file, [[
            #include <Concerto/Core/Types/Types.hpp>
            extern "C" {
                void CCT_EXPORT Dummy() {}
                int CCT_EXPORT DummyInt() { return 42;}
                int CCT_EXPORT Increment(int v) { return v + 1;}
                int CCT_EXPORT GlobalInt = 42;
            }
            ]]
        )
    end)
end)

target("concerto-core-tests", function()
    add_deps("concerto-core", "concerto-core-dummy")
    set_kind("binary")
    if (is_mode("debug")) then
        set_symbols("debug")
    end
    set_warnings("allextra")
    set_languages("cxx20")

    add_files("*.cpp")
    add_packages("catch2")

    if has_config("unitybuild") then
        add_rules("c++.unity_build", {batchsize = 12, uniqueid = "CONCERTO_UNITY_BUILD_ID"})
    end

    if is_plat("linux") then
        add_syslinks("dl")
    end

    add_cxxflags("cl::/wd4251") -- needs to have dll-interface to be used by clients of
end)