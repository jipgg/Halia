local Matrix3 = std.math.Matrix3
local Vector2 = std.math.Vector2
local File_path = std.filesystem.File_path
local feedback = std.process.execute("echo",  "hello world");
type Config = {
    runtime: "console" | "desktop",
}

-- local valid, data = pcall(Child, "aaaaaaaaa", "libe-");
-- if valid then
--     print(data.pid)
-- else
--     print(data)
-- end

--print(std.process.args:at(0), std.process.args:at(1));

type Arg_data = {
    data: string?,
    var: string?,
    v: string?,
}
std.task.spawn(function()
    print("spawn does something")
end)
std.task.spawn(function()
    std.task.wait(4)
    print("task does something delayed");
end)

print(std.process.find_in_path_environment("zig"))
print(feedback);
print(Vector2(10, 10) + Vector2(10, 20))
local ex = std.process.execute("cmake", "--version");
print(std.process.exists_in_path_environment("cmake.exe"));
print(std.process.exists_in_path_environment("zig"));
print(ex.error);

local my_path = File_path("main.luau");
print(std.filesystem.exists(my_path), tostring(my_path), tostring(my_path:stem()))
print(std.math.Vector2)
local b = std.math.Vector2(2, 1)
local a = std.math.Vector3(0, 1, 2)
local m = Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9)
std.task.wait(6)
local x = std.do_error()
local e = Matrix3.from_scale(Vector2(0, 0))
print(a, b, m, e)
for i, v in std.math do
    print(`{i} = {v}`);
end

