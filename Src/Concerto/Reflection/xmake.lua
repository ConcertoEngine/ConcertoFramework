add_requires("toml11")
add_requires("libllvm", {configs = {clang = true} })
add_requires("cxxopts")
add_requires("simdjson", { configs = { threads = true, noexceptions = false, logging = false } })

target("concerto-reflection", function()
    set_kind("$(kind)")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_defines("CCT_REFLECTION_BUILD", { public = false })
    add_includedirs("../../../Src/", { public = true })
    add_packages("simdjson")
    if is_kind("static") then
        add_defines("CCT_REFLECTION_STATIC", { public = true })
    end
    if is_plat("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
    end
    local files = {
        ".",
        "Class",
        "Enumeration",
        "EnumIterator",
        "EnumValue",
        "GenericClass",
        "GlobalNamespace",
        "Json",
        "MemberVariable",
        "Method",
        "Namespace",
        "Object",
        "Package",
        "PackageLoader",
        "Registry",
        "TemplateClass",
        "Signal",
        "String",
        "Vector",
    }
    for _, dir in ipairs(files) do
        add_files_to_target("./" .. dir, false, true)
    end
    add_deps("concerto-pkg-generator", {links = false, inherit = false})
    add_deps("concerto-header-plugin", {links = false, plugin = "pkg-generator", inherit = false})
    add_deps("concerto-cpp-plugin", {links = false, plugin = "pkg-generator", inherit = false})
    add_deps("concerto-core", { public = true })
    add_rules("cct_cpp_reflect")

    if is_mode("debug") then
        set_symbols("debug")
    end
    set_policy("build.across_targets_in_parallel", false)
    add_cxxflags("cl::/Zc:preprocessor")
end)
