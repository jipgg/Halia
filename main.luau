local Matrix3 = std.math.Matrix3
local Vector2 = std.math.Vector2
local File_path = std.filesystem.File_path
local proc = std.process
local co_spawn, co_wait = std.co_spawn, std.co_wait
local feedback = proc.execute("echo",  "hello world");
std.Event()
std.process.Child_process("a");
local Child_process = proc.Child_process
Child_process("aaaa")
--local Console_color = std.io.Console_color
--std.io.console_color = Console_color.Red
local e: Event<number> = std.Event();
e:fire(1)
local disc = e:connect(function(a0: number)  print("ey", a0) end)
e:fire(2)
e:disconnect(disc)

co_spawn(function()
    co_wait(15)
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
    print("\033[31mfinished!!!!!!!!!!!!")
end)
print(std.process.find_in_path_environment("zig"))
print(feedback);
print(Vector2(10, 10) + Vector2(10, 20))
local ex = proc.execute("cmake", "--version");
print(proc.exists_in_path_environment("cmake.exe"));
print(proc.exists_in_path_environment("zig"));
print(ex.error);

local my_path = File_path("main.luau");
print(std.filesystem.exists(my_path), tostring(my_path), tostring(my_path:stem()))
print(std.math.Vector2)
local b = std.math.Vector2(2, 1)
local a = std.math.Vector3(0, 1, 2)
local m = Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9)
co_wait(6)
local x = std.do_error()
co_wait(5)
local h = Matrix3.from_scale(Vector2(0, 0))
print(a, b, m, e)
for i, v in std.math do
    print(`{i} = {v}`);
end

