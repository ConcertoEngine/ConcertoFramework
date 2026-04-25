if is_mode("coverage") then
	if not is_plat("windows") then
		add_links("gcov")
	end
end

if has_config("tests") then
    add_requires("catch2")
	includes("Core/xmake.lua")
	includes("Reflection/xmake.lua")

	target("concerto-tests", function()
		set_kind("binary")
		add_files("./main.cpp")
		add_packages("catch2", {public = true})
		add_deps("concerto-core-tests", {public = true})
		add_deps("concerto-reflection-tests", {public = true})
	end)
end