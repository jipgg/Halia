export type Span<T> = {T}
declare class Args_span
    size: number
    function at(self, index: number): string
    function parse_params(self): {[string]: string}
end
declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | File_path
export type Copy_options = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class File_path
    function stem(self): File_path
    function is_empty(self): boolean
    function file_name(self): File_path
    function has_stem(self): boolean
    function root_path(self): File_path
    function parent_path(self): File_path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): File_path
    function replace_extension(self, s: string)
    function relative_path(self): File_path
    function compare(self): number
    function root_name(self): File_path
    function root_directory(self): File_path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): File_path
end
declare class Directory_entry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): File_path
    function is_socket(self): boolean
    function is_other(self): boolean
    function is_symlink(self): boolean
    function is_block_file(self): boolean
    function is_regular_file(self): boolean
    function is_character_file(self): boolean
end
declare class Vector2
    x: number
    y: number
    function dot(self, a: Vector2): number
    function normalized(self): Vector2
    function norm(self): number
    function abs(self): Vector2
    function __add(self, a: Vector2): Vector2
    function __mul(self, scalar: number): Vector2
    function __div(self, scalar: number): Vector2
    function __sub(self, a: Vector2): Vector2
    function __unm(self): Vector2
    function __call(self, index: number): number
end
declare class Matrix3
    function inverse(self): Matrix3
    function transpose(self): Matrix3
    __mul: ((Matrix3, Matrix3)->Matrix3)
        & ((Matrix3, Vector3)->Vector3)
    function __add(self, other: Matrix3): Matrix3
    function __sub(self, other: Matrix3): Matrix3
end
declare class Vector3
    x: number
    y: number
    z: number
    function dot(self, a: Vector3): number
    function normalized(self): Vector3
    function norm(self): number
    function abs(self): Vector3
    function __add(self, a: Vector3): Vector3
    function __mul(self, scalar: number): Vector3
    function __div(self, scalar: number): Vector3
    function __sub(self, a: Vector3): Vector3
    function __unm(self): Vector3
end
export type filesystem = {
    create_directory: (where: PathUnion) -> boolean,
    exists: (path: PathUnion) -> boolean,
    is_character_file: (path: PathUnion) -> boolean,
    copy_file: (from: PathUnion, to: PathUnion, opt: Copy_options?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: Copy_options?) -> (),
    absolute: (path: PathUnion) -> File_path,
    get_children_of: (path: PathUnion) -> {Directory_entry},
    get_descendants_of: (path: PathUnion) -> {Directory_entry},
    exe_path: ()->File_path,
    current_path: ()->File_path,
    canonical: (path: File_path)->File_path,
    proximate: (path: File_path, base: File_path?)->File_path,
    create_symlink: (to: File_path, new_symlink: File_path)->(),
    relative: (path: File_path, base: File_path?)->File_path,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    File_path: (path: string)->File_path,
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
    from_scale = function(scale_factor: Vector2 | number): Matrix3 return {} :: any end,
    from_rotation = function(radians: number): Matrix3 return {} :: any end,
    from_translation = function(translation: Vector2): Matrix3 return {} :: any end,
}, {__call = function(...): Matrix3 return {} :: any end}))
export type math = {
    Vector2: (x: number?, y: number?)->Vector2,
    Vector3: (x: number?, y: number?, z: number?)->Vector3,
    Matrix3: ((row1: {number}, row2: {number}, row3: {number})->Matrix3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->Matrix3)
        & Matrix3_named_constructors, 
}
declare class Execution_feedback
    output: string?
    error: string?
    exit_code: number
    failed_before_executing: boolean
end
declare class Process_id
    function __tostring(self): string
end
declare class Child_process
    pid: Process_id
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
    args: Args_span,
    execute: (exe: string, ...string)->Execution_feedback,
    exists_in_path_environment: (what: string)->boolean,
    find_in_path_environment: (what: string)->string?,
    Child_process: ((exe: string, ...string)->Child_process)
        & ((pid: Process_id)->Child_process),
    parse_args: (args: {string}) -> unknown,
}
export type Halia_standard_library = {
        filesystem: filesystem,
        math: math,
        process: process,
        co_spawn: <Args...>((Args...)->(), Args...) -> thread,
        co_wait: (seconds: number)->(),
        terminate: (exit_code: number?)->(),
        Event: <Params...>()->Event<Params...>,
}
declare std: Halia_standard_library
