print("hello world")
local Path = waw.fs.Path
local fs = waw.fs;

local my_path = Path("main.luau");
print(fs.exists(my_path), tostring(my_path), tostring(my_path:stem()))
