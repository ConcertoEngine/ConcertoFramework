function add_files_to_target(p, hpp_as_files, install)
    for _, dir in ipairs(os.filedirs(p)) do
        relative_dir = path.relative(dir, "Src/")
        if os.isdir(dir) then
            add_files(path.join("Src", relative_dir, "**.cpp"))
            if hpp_as_files then
                add_files(path.join("Src", relative_dir, "**.hpp"))
            end
            install = path.basename(dir) ~= "Private"
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.hpp)"), {install = install})
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.inl)"), {install = install})
        else
            local ext = path.extension(relative_dir)
            if ext == ".hpp" or ext == ".inl" then
                add_headerfiles(path.join("Src", "(" .. relative_dir .. ")"), {install = install})
            elseif ext == ".cpp" then
                add_files(path.join("Src", relative_dir))
            end
        end
    end
end

includes("Src/Concerto/Core/xmake.lua")
includes("Tests/xmake.lua")
