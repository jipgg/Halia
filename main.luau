local Matrix3 = std.math.Matrix3
local Vector2 = std.math.Vector2
local File_path = std.filesystem.File_path
local co_spawn, co_wait = std.co_spawn, std.co_wait
local feedback = std.process.execute("echo",  "hello world");

co_spawn(function()
    co_wait(15)-- this throws a luaC_threadbarrier debug break
    -- probably cause it gets gc'd
    -- try holding a std::unique_ptr<int, std::function<void(int*)>> of a lua_ref
    -- to persist ownership in cpp
    print("spawn does something")
end)
co_spawn(function(a: string)
    co_wait(4)
    print("task does something delayed", a);
end, "AAAAAA")

co_spawn(function()
    local acc = 0
    while acc < 10 do
        co_wait(1)
        acc += 1
    end
    print("finished!!!!!!!!!!!!")
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
co_wait(6)
--local x = std.do_error()
local e = Matrix3.from_scale(Vector2(0, 0))
print(a, b, m, e)
for i, v in std.math do
    print(`{i} = {v}`);
end

