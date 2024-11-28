print("hello world")
local Path = waw.fs.Path
local fs = waw.fs;
type Mat3 = math_Matrix3
local Mat3 = waw.math.Matrix3
local Vec2 = waw.math.Vector2

local my_path = Path("main.luau");
print(fs.exists(my_path), tostring(my_path), tostring(my_path:stem()))
print(waw.math.Vector2)
local b = waw.math.Vector2(2, 1)
local a = waw.math.Vector3(0, 1, 2)
local m = Mat3(1, 2, 3, 4, 5, 6, 7, 8, 9)
local e = Mat3.from_scale(Vec2(0, 0))
print(a, b, m, e)
for i, v in waw.math do
    print(`{i} = {v}`);
end

