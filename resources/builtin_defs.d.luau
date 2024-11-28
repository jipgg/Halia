declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | fs_Path
export type fs_Copy_options = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class fs_Path
    function stem(self): fs_Path
    function is_empty(self): boolean
    function file_name(self): fs_Path
    function has_stem(self): boolean
    function root_path(self): fs_Path
    function parent_path(self): fs_Path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): fs_Path
    function replace_extension(self, s: string)
    function relative_path(self): fs_Path
    function compare(self): number
    function root_name(self): fs_Path
    function root_directory(self): fs_Path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): fs_Path
end
declare class fs_Directory_entry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): fs_Path
    function is_socket(self): boolean
    function is_other(self): boolean
    function is_symlink(self): boolean
    function is_block_file(self): boolean
    function is_regular_file(self): boolean
    function is_character_file(self): boolean
end
declare class math_Vector2
    x: number
    y: number
    function dot(self, a: math_Vector2): number
    function normalized(self): math_Vector2
    function norm(self): number
    function abs(self): math_Vector2
    function __add(self, a: math_Vector2): math_Vector2
    function __mul(self, scalar: number): math_Vector2
    function __div(self, scalar: number): math_Vector2
    function __sub(self, a: math_Vector2): math_Vector2
    function __unm(self): math_Vector2
    function __call(self, index: number): number
end
declare class math_Matrix3
    function inverse(self): math_Matrix3
    function transpose(self): math_Matrix3
    __mul: ((math_Matrix3, math_Matrix3)->math_Matrix3)
        & ((math_Matrix3, math_Vector3)->math_Vector3)
    function __add(self, other: math_Matrix3): math_Matrix3
    function __sub(self, other: math_Matrix3): math_Matrix3
end
declare class math_Vector3
    x: number
    y: number
    z: number
    function dot(self, a: math_Vector3): number
    function normalized(self): math_Vector3
    function norm(self): number
    function abs(self): math_Vector3
    function __add(self, a: math_Vector3): math_Vector3
    function __mul(self, scalar: number): math_Vector3
    function __div(self, scalar: number): math_Vector3
    function __sub(self, a: math_Vector3): math_Vector3
    function __unm(self): math_Vector3
end
export type filesystem = {
    create_directory: (where: PathUnion) -> boolean,
    exists: (path: PathUnion) -> boolean,
    is_character_file: (path: PathUnion) -> boolean,
    copy_file: (from: PathUnion, to: PathUnion, opt: fs_Copy_options?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: fs_Copy_options?) -> (),
    absolute: (path: PathUnion) -> fs_Path,
    get_children_of: (path: PathUnion) -> {fs_Directory_entry},
    get_descendants_of: (path: PathUnion) -> {fs_Directory_entry},
    exe_path: ()->fs_Path,
    current_path: ()->fs_Path,
    canonical: (path: fs_Path)->fs_Path,
    proximate: (path: fs_Path, base: fs_Path?)->fs_Path,
    create_symlink: (to: fs_Path, new_symlink: fs_Path)->(),
    relative: (path: fs_Path, base: fs_Path?)->fs_Path,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    Path: (path: string)->fs_Path,
}
declare class Event_connection_id
    function __tostring(self): string
end
export type Event<Ts...> = {
    connect: (self: Event<Ts...>, (Ts...)->())->Event_connection_id,
    disconnect: (self: Event<Ts...>, id: Event_connection_id)->(),
    fire: (self: Event<Ts...>, Ts...)->(),
}
type math_Matrix3_named_constructors = typeof(setmetatable({
    from_scale = function(scale_factor: math_Vector2 | number): math_Matrix3 return {} :: any end,
    from_rotation = function(radians: number): math_Matrix3 return {} :: any end,
    from_translation = function(translation: math_Vector2): math_Matrix3 return {} :: any end,
}, {__call = function(...): math_Matrix3 return {} :: any end}))
export type math = {
    Vector2: (x: number?, y: number?)->math_Vector2,
    Vector3: (x: number?, y: number?, z: number?)->math_Vector3,
    Matrix3: ((row1: {number}, row2: {number}, row3: {number})->math_Matrix3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->math_Matrix3)
        & math_Matrix3_named_constructors, 
}
export type LuwawRT = {
        fs: filesystem,
        math: math,
        Event: <Params...>()->Event<Params...>,
}
declare waw: LuwawRT
