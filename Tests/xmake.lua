if is_mode("coverage") then
	if not is_plat("windows") then
		add_links("gcov")
	end
end

if has_config("tests") then
    add_requires("catch2")
	includes("Core/xmake.lua")
end