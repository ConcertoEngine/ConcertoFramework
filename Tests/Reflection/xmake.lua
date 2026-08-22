
if has_config("tests") and has_config("reflection") then
    target("concerto-reflection-tests", function () 
        set_kind("object")
        set_languages("cxx20")
        add_rpathdirs("$ORIGIN")
        add_files("*.cpp", "*.refl.hpp")
        add_packages("catch2", "toml11")
        add_deps("concerto-reflection")
        add_deps("concerto-plugin-api", {inherit = false})
        add_links("concerto-plugin-api")
        add_linkdirs("$(builddir)/$(plat)/$(arch)/$(mode)")
        add_includedirs("../../Src")
        add_rules("cct_cpp_reflect")
        add_includedirs(".", { public = true }) -- temporary
        add_includedirs("../", { public = true })
        add_headerfiles("**.hpp")
        add_defines("CCT_REFLECTION_TESTS_BUILD", { public = false })
        add_defines("CCT_ENABLE_ASSERTS")
        if is_plat("windows") then
            add_cxflags("/Zc:preprocessor")
        end

        if is_mode("debug") then
            set_symbols("debug")
        end

        on_config(function(package)
            import("core.project.project")
            local llvm = project.required_package("libllvm")
            assert(llvm, "libllvm not found!")
            local llvm_lib = path.join(llvm:installdir(), "lib")
            package:add("rpathdirs", llvm_lib)
        end)
    end)
end
    
