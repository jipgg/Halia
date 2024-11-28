print("hello world")
local Path = waw.filesystem.Path
local fs = waw.filesystem;
local Matrix3 = waw.math.Matrix3
local Vector2 = waw.math.Vector2
local status = waw.process.system("cmd echo hello world");
print(status.stdout, status.exit_code)
print("echoed")
print(typeof({}::Path));

local my_path = Path("main.luau");
print(fs.exists(my_path), tostring(my_path), tostring(my_path:stem()))
print(waw.math.Vector2)
local b = waw.math.Vector2(2, 1)
local a = waw.math.Vector3(0, 1, 2)
local m = Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9)
local e = Matrix3.from_scale(Vector2(0, 0))
print(a, b, m, e)
for i, v in waw.math do
    print(`{i} = {v}`);
end

