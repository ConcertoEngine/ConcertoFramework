option("examples", { description = "Build examples files", default = true })

if (has_config("examples")) then
    includes("RenderGraph/xmake.lua")
end