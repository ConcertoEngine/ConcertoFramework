rule("cct_cpp_reflect")
    add_deps("find_cct_pkg_generator")
    set_extensions(".refl.hpp")

    on_config(function (target)
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local generatedCpp = path.join(target:autogendir(), targetName .. "Package.gen.cpp")

        target:add("headerfiles", path.join(target:autogendir(), "(" .. targetName .. "Package.gen.hpp)"))
        target:add("files", generatedCpp, {always_added = true})
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
        collect_plugins(target, plugins)

        function process_target(t, args, is_root)
            for _, define in ipairs(t:get("defines")) do
                local conf = (t:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false or is_root then
                    table.insert(args, "-D" .. define)
                end
            end

            for _, include in ipairs(t:get("includedirs")) do
                table.insert(args, "-I" .. include)
            end
            for _, dep in ipairs(t:get("deps")) do
                local project_target = project.target(dep)
                if project_target then
                    process_target(project_target, args, false)
                end
            end

            for pkgname, pkgconf in pairs(t:extraconf("packages") or {}) do
                local required_package = project.required_package(pkgname)
                if required_package then
                    process_package(required_package, args)
                end
            end
        end

        function process_package(p, args)
            for _, inc in ipairs(p:get("sysincludedirs") or {}) do
                table.insert(args, "-I" .. inc)
            end
            for _, define in ipairs(p:get("defines")) do
                local conf = (p:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false then
                    table.insert(args, "-D" .. define)
                end
            end
        end

        process_target(target, args, true)

        for _, header in ipairs(target:get("headerfiles")) do
            header = header:gsub("[%(%)]", "")
            for _, file_path in ipairs(os.filedirs(header)) do
                file_path = path.normalize(file_path)
                for _, incdir in ipairs(target:get("includedirs")) do
                    incdir = path.normalize(incdir)
                    if file_path:startswith(incdir) then
                        file_path = file_path:sub(#incdir + 2)
                        break
                    end
                end
                if file_path:endswith(".refl.hpp") then
                    file_path = file_path:gsub("\\", "/")
                    table.insert(args, "-H" .. file_path)
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
        batchcmds:set_depmtime(os.mtime(outputCppFile))
        batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)
