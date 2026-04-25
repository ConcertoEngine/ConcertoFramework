add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
set_runtimes(is_mode("debug") and "MDd" or "MD")

function add_files_to_target(p, hpp_as_files, install)
    for _, dir in ipairs(os.filedirs(p)) do
        local relative_dir = path.relative(dir, path.join(os.projectdir(), "Src"))
        if os.isdir(dir) then
            add_files(path.join("$(projectdir)", "Src", relative_dir, "**.cpp"))
            if hpp_as_files then
                add_files(path.join("$(projectdir)", "Src", relative_dir, "**.hpp"))
            end
            add_files(path.join("$(projectdir)", "Src", relative_dir, "*.refl.hpp"))
            local should_install = path.basename(dir) ~= "Private"
            add_headerfiles(path.join("$(projectdir)", "Src", "(" .. relative_dir .. "/*.hpp)"), {install = should_install})
            add_headerfiles(path.join("$(projectdir)", "Src", "(" .. relative_dir .. "/*.inl)"), {install = should_install})
        else
            local ext = path.extension(relative_dir)
            if ext == ".hpp" or ext == ".inl" then
                add_headerfiles(path.join("$(projectdir)", "Src", "(" .. relative_dir .. ")"), {install = install})
            elseif ext == ".cpp" then
                add_files(path.join("$(projectdir)", "Src", relative_dir))
            end
        end
    end
end

option("reflection", { description = "Enable C++ reflection system", default = true })
option("profiling", { description = "Build with tracy profiler", default = false })
option("graphics", { description = "Build graphics", default = false })

includes("Xmake/**.lua")
includes("Xmake/Rules/**.lua")
includes("Src/Concerto/Core/xmake.lua")
includes("Src/Concerto/Profiler/xmake.lua")

if has_config("graphics") then
    includes("Src/Concerto/Graphics/xmake.lua")
    includes("Examples/Graphics/xmake.lua")
end

if has_config("reflection") then
    includes("Src/Concerto/HeaderPlugin/xmake.lua")
    includes("Src/Concerto/CppPlugin/xmake.lua")
    includes("Src/Concerto/PackageGenerator/xmake.lua")
    includes("Src/Concerto/Reflection/xmake.lua")
end

includes("Tests/xmake.lua")
