declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | filesystem_Path
export type filesystem_Copy_options = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class filesystem_Path
    function stem(self): filesystem_Path
    function is_empty(self): boolean
    function file_name(self): filesystem_Path
    function has_stem(self): boolean
    function root_path(self): filesystem_Path
    function parent_path(self): filesystem_Path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): filesystem_Path
    function replace_extension(self, s: string)
    function relative_path(self): filesystem_Path
    function compare(self): number
    function root_name(self): filesystem_Path
    function root_directory(self): filesystem_Path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): filesystem_Path
end
declare class filesystem_Directory_entry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): filesystem_Path
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
    copy_file: (from: PathUnion, to: PathUnion, opt: filesystem_Copy_options?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: filesystem_Copy_options?) -> (),
    absolute: (path: PathUnion) -> filesystem_Path,
    get_children_of: (path: PathUnion) -> {filesystem_Directory_entry},
    get_descendants_of: (path: PathUnion) -> {filesystem_Directory_entry},
    exe_path: ()->filesystem_Path,
    current_path: ()->filesystem_Path,
    canonical: (path: filesystem_Path)->filesystem_Path,
    proximate: (path: filesystem_Path, base: filesystem_Path?)->filesystem_Path,
    create_symlink: (to: filesystem_Path, new_symlink: filesystem_Path)->(),
    relative: (path: filesystem_Path, base: filesystem_Path?)->filesystem_Path,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    Path: (path: string)->filesystem_Path,
}
declare class Event_connection
    function __tostring(self): string
end
export type Event<Ts...> = {
    connect: (self: Event<Ts...>, (Ts...)->())->Event_connection,
    disconnect: (self: Event<Ts...>, id: Event_connection)->(),
    fire: (self: Event<Ts...>, Ts...)->(),
}
type Matrix3_named_constructors = typeof(setmetatable({
    from_scale = function(scale_factor: math_Vector2 | number): math_Matrix3 return {} :: any end,
    from_rotation = function(radians: number): math_Matrix3 return {} :: any end,
    from_translation = function(translation: math_Vector2): math_Matrix3 return {} :: any end,
}, {__call = function(...): math_Matrix3 return {} :: any end}))
export type math = {
    Vector2: (x: number?, y: number?)->math_Vector2,
    Vector3: (x: number?, y: number?, z: number?)->math_Vector3,
    Matrix3: ((row1: {number}, row2: {number}, row3: {number})->math_Matrix3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->math_Matrix3)
        & Matrix3_named_constructors, 
}
declare class process_Execution_feedback
    output: string?
    error: string?
    exit_code: number
    failed_before_executing: boolean
end
declare class process_Pid
    function __tostring(self): string
end
export type Reader_interface = {
    read_line: () -> string?,
    read: () -> string?,
    eof: () -> boolean,
}
declare class process_Child
    pid: process_Pid
    valid: boolean
    exit_code: number
    native_exit_code: number
    function join(self)
    function detach(self)
    function wait(self)
    function joinable(self): boolean
    function running(self): boolean
    function terminate(self)
end
export type process = {
    args: {string},
    execute: (exe: string, ...string)->process_Execution_feedback,
    exists_in_path_environment: (what: string)->boolean,
    find_in_path_environment: (what: string)->string?,
    Child: ((exe: string, ...string)->process_Child)
        & ((pid: process_Pid)->process_Child),
    parse_args: (args: {string}) -> unknown,
}
export type Halia_standard_library = {
        filesystem: filesystem,
        math: math,
        process: process,
        Event: <Params...>()->Event<Params...>,
}
declare std: Halia_standard_library
