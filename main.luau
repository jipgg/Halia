local Path = std.filesystem.Path
local fs = std.filesystem;
local Matrix3 = std.math.Matrix3
local Vector2 = std.math.Vector2
--local success, callback_or_error = pcall(std.process.run, "cmd echo hello world");
--if success then
--print(callback_or_error.stdout, callback_or_error.exit_code)
--print("abc")
--else print(`error {callback_or_error}`)
--end
local feedback = std.process.execute("echo",  "hello world");
print(std.process.find_in_path_environment("zig"))
print(feedback);
print(Vector2(10, 10) + Vector2(10, 20))
local ex = std.process.execute("cmake", "--version");
print(std.process.exists_in_path_environment("cmake.exe"));
print(std.process.exists_in_path_environment("zig"));
print(ex.error);

local my_path = Path("main.luau");
print(fs.exists(my_path), tostring(my_path), tostring(my_path:stem()))
print(std.math.Vector2)
local b = std.math.Vector2(2, 1)
local a = std.math.Vector3(0, 1, 2)
local m = Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9)
local e = Matrix3.from_scale(Vector2(0, 0))
print(a, b, m, e)
for i, v in std.math do
    print(`{i} = {v}`);
end

