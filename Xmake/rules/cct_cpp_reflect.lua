rule("cct_cpp_reflect")  
    set_extensions(".refl.hpp")  
  
    on_config(function (target)  
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)  
        local generatedCpp = path.join(target:autogendir(), targetName .. "Package.gen.cpp")  
  
        target:add("headerfiles", path.join(target:autogendir(), "(" .. targetName .. "Package.gen.hpp)"))  
        local generatedAcceptCpp = path.join(target:autogendir(), targetName .. "Package.Accept.gen.cpp")
        target:add("files", generatedCpp, {always_added = true})
        target:add("files", generatedAcceptCpp, {always_added = true})
        target:add("includedirs", target:autogendir(), {public = true})  
        target:add("defines", path.basename(targetName):upper() .. "_BUILD", { public = false })  
  
        local upperPackageName = path.basename(targetName):upper()  
        target:add("defines", upperPackageName .. "PACKAGE_BUILD", { public = false })  
        if target:kind() == "static" then  
            target:add("defines", upperPackageName .. "PACKAGE_STATIC", { public = true })  
        end  
    end)
  
    before_buildcmd_files(function (target, batchcmds, sourcebatch, opt)  
        import("core.project.project")  
        import("core.language.language")  
        import("core.tool.compiler")  
        import("core.tool.toolchain")
        import("lib.detect.find_tool")
        import("lib.detect.find_library")

        local function find_cct_pkg_generator(target)
            local cctPkgGen = project.required_package("concerto-reflection")
            local dir
            if cctPkgGen then
                if cctPkgGen:installdir() then
                    dir = path.join(cctPkgGen:installdir(), "bin")
                else
                    dir = cctPkgGen:get("bindirs")
                end
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
            print("Looking for concerto-pkg-generator in " .. dir)
            local concerto_pkg_generator = find_tool("concerto-pkg-generator", {check = "-v", paths = {dir}})
            assert(concerto_pkg_generator, "concerto-pkg-generator not found! Please install concerto-reflection package or build concerto-pkg-generator target.")
            target:data_set("concerto-pkg-generator", concerto_pkg_generator)
            
            local concerto_header_plugin = find_library("concerto-header-plugin", dir, {kind = "shared"})
            assert(concerto_header_plugin, "concerto-header-plugin not found! Please install concerto-reflection package or build concerto-header-plugin target.")

            local concerto_cpp_plugin = find_library("concerto-cpp-plugin", dir, {kind = "shared"})
            assert(concerto_cpp_plugin, "concerto-cpp-plugin not found! Please install concerto-reflection package or build concerto-cpp-plugin target.")
            target:data_set("concerto-pkg-generator-plugins", {path.join(dir, concerto_header_plugin.filename), path.join(dir, concerto_cpp_plugin.filename)})
        end
        
        find_cct_pkg_generator(target)

        local cctPkgGen = target:data("concerto-pkg-generator")  
        assert(cctPkgGen, "concerto-pkg-generator not found!")  
        local envs = target:data("concerto-pkg-generator-envs")  
  
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)  
        local outputCppFile = path.join(target:autogendir(), targetName .. "Package.gen.cpp")  
        local outHppFile = path.join(target:autogendir(), targetName .. "Package.gen.hpp")  
  
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection." .. target:name())  
        local args = { target:autogendir() }  
  
        table.insert(args, "-DCCT_REFLECTION_PKG_GENERATOR_BUILD")  
        for _, header in ipairs(sourcebatch.sourcefiles) do  
            table.insert(args, "-s" .. header)  
        end  
  
        local function collect_plugins(t, plugins)  
            for _, dep in ipairs(t:get("deps") or {}) do  
                local dep_target = project.target(dep)  
                if dep_target then  
                    local plugin_type = t:extraconf("deps", dep, "plugin")  
                    if plugin_type == "pkg-generator" and dep_target:kind() == "shared" then  
                        local plugin_path = dep_target:targetfile()  
                        if plugin_path and not plugins[plugin_path] then  
                            plugins[plugin_path] = true  
                            table.insert(args, "-P" .. plugin_path)  
                        end  
                    end  
                    collect_plugins(dep_target, plugins)  
                end  
            end  
        end  
  
        local plugins = {}
        local plugins_ordered = {}
        collect_plugins(target, plugins)
        for plugin_path, _ in pairs(plugins) do
            table.insert(plugins_ordered, plugin_path)
        end
        table.sort(plugins_ordered)
        for _, plugin in ipairs(target:data("concerto-pkg-generator-plugins") or {}) do
            if not plugins[plugin] then
                table.insert(args, "-P" .. plugin)
                table.insert(plugins_ordered, plugin)
            end
        end
  
        local visited_targets = {}
        local visited_packages = {}
        local seen_includes = {}
        local seen_defines = {}

        local function add_include(a, inc)
            if not seen_includes[inc] then
                seen_includes[inc] = true
                table.insert(a, "-I" .. inc)
            end
        end

        local function add_define(a, def)
            if not seen_defines[def] then
                seen_defines[def] = true
                table.insert(a, "-D" .. def)
            end
        end

        function process_target(t, a, is_root)
            if not is_root then
                if visited_targets[t:name()] then return end
                visited_targets[t:name()] = true
            end

            for _, define in ipairs(t:get("defines")) do
                local conf = (t:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false or is_root then
                    add_define(a, define)
                end
            end

            for _, include in ipairs(t:get("includedirs")) do
                add_include(a, include)
            end
            for _, dep in ipairs(t:get("deps")) do
                local project_target = project.target(dep)
                if project_target then
                    process_target(project_target, a, false)
                end
            end

            for pkgname, pkgconf in pairs(t:extraconf("packages") or {}) do
                local required_package = project.required_package(pkgname)
                if required_package then
                    process_package(required_package, a)
                end
            end
        end

        function process_package(p, a)
            if visited_packages[p:name()] then return end
            visited_packages[p:name()] = true

            for _, inc in ipairs(p:get("sysincludedirs") or {}) do
                add_include(a, inc)
            end
            for _, define in ipairs(p:get("defines")) do
                local conf = (p:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false then
                    add_define(a, define)
                end
            end
        end

        process_target(target, args, true)
  
        local refl_headers = {}
        local refl_headers_set = {}
        for _, header in ipairs(target:get("headerfiles")) do
            header = header:gsub("[%(%)]", "")
            for _, abs_file_path in ipairs(os.filedirs(header)) do
                local file_path = abs_file_path:gsub("\\", "/")
                local best_len = 0
                for _, incdir in ipairs(target:get("includedirs")) do
                    local norm_incdir = incdir:gsub("\\", "/"):gsub("/$", "")
                    if file_path:startswith(norm_incdir .. "/") and #norm_incdir > best_len then
                        file_path = file_path:sub(#norm_incdir + 2)
                        best_len = #norm_incdir
                    end
                end
                if file_path:endswith(".refl.hpp") and not refl_headers_set[file_path] then
                    refl_headers_set[file_path] = true
                    table.insert(args, "-H" .. file_path)
                    table.insert(refl_headers, abs_file_path)
                end
            end
        end
  
        local libllvm = project.required_package("libllvm")  
        assert(libllvm, "libllvm not found!")  
        local llvm_include = path.join(libllvm:installdir(), "lib", "clang", libllvm:version():major(), "include")  
        if is_plat("macosx") then  
            local xcode = target:toolchain("xcode")  
            assert(xcode, "xcode toolchain not found!")  
            local sdk_path = xcode:config("xcode_sysroot")  
            assert(sdk_path, "Unable to determine SDK path from xcode toolchain")  
            table.insert(args, "-S" .. sdk_path)  
        end  
        table.insert(args, "-R" .. path.join(libllvm:installdir(), "lib", "clang", libllvm:version():major()))  
  
        batchcmds:vrunv(cctPkgGen.program, args, {envs = envs})

        batchcmds:add_depfiles(sourcebatch.sourcefiles)
        batchcmds:add_depfiles(plugins_ordered)
        batchcmds:add_depfiles(refl_headers)
  
        local lastmtime = math.min(os.mtime(outputCppFile), os.mtime(outHppFile))  
        batchcmds:set_depmtime(lastmtime)  
        batchcmds:set_depcache(target:dependfile(outputCppFile))  
    end)