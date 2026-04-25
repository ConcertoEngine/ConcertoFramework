rule("find_cct_pkg_generator")
    before_build(function (target)
        import("core.project.project")
        import("lib.detect.find_tool")

        local cctPkgGen = project.required_package("concerto-reflection")
        local dir
        if cctPkgGen then
            dir = path.join(cctPkgGen:installdir(), "bin")
        else
            cctPkgGen = project.target("concerto-pkg-generator")
            if cctPkgGen then
                dir = cctPkgGen:targetdir()
            else
                raise("Concerto Reflection package not found")
            end
        end
        
        if not dir then
            raise("Unable to locate concerto-pkg-generator")
        end

        local program = find_tool("concerto-pkg-generator", {check = "-v", paths = {dir}})
        assert(program, "concerto-pkg-generator not found! Please install concerto-reflection package or build concerto-pkg-generator target.")
        target:data_set("concerto-pkg-generator", program)
    end)