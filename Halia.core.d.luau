export type Span<T> = {T}
declare class ArgsSpan
    size: number
    function at(self, index: number): string
    function parse_params(self): {[string]: string}
    function parse_flags(self): {[string]: boolean}
    function totable(self): {string}
end
declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | FilePath
export type CopyOptions = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class FilePath
    function stem(self): FilePath
    function is_empty(self): boolean
    function file_name(self): FilePath
    function has_stem(self): boolean
    function root_path(self): FilePath
    function parent_path(self): FilePath
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): FilePath
    function replace_extension(self, s: string)
    function relative_path(self): FilePath
    function compare(self): number
    function root_name(self): FilePath
    function root_directory(self): FilePath
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): FilePath
end
declare class DirectoryEntry
    function is_directory(self): boolean
function is_fifo(self): boolean
    function path(self): FilePath
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
export type stdfilesystem = {
    create_directory: (where: PathUnion) -> boolean,
    exists: (path: PathUnion) -> boolean,
    is_character_file: (path: PathUnion) -> boolean,
    copy_file: (from: PathUnion, to: PathUnion, opt: CopyOptions?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: CopyOptions?) -> (),
    absolute: (path: PathUnion) -> FilePath,
    get_children_of: (path: PathUnion) -> {DirectoryEntry},
    get_descendants_of: (path: PathUnion) -> {DirectoryEntry},
    exe_path: ()->FilePath,
    current_path: ()->FilePath,
    canonical: (path: FilePath)->FilePath,
    proximate: (path: FilePath, base: FilePath?)->FilePath,
    create_symlink: (to: FilePath, new_symlink: FilePath)->(),
    relative: (path: FilePath, base: FilePath?)->FilePath,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    Path: ((path: string)->FilePath),
}
declare class EventConnection
    function __tostring(self): string
end
export type Event<Ts...> = {
    connect: (self: Event<Ts...>, (Ts...)->())->EventConnection,
    disconnect: (self: Event<Ts...>, id: EventConnection)->(),
    fire: (self: Event<Ts...>, Ts...)->(),
}
export type stdmath = {
    Vector2: ((x: number?, y: number?)->Vector2),
    Vector3: ((x: number?, y: number?, z: number?)->Vector3),
    Matrix3: ((row1: {number}, row2: {number}, row3: {number})->Matrix3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->Matrix3)
}
declare class ExecutionFeedback
    output: string?
    error: string?
    exit_code: number
    failed_before_executing: boolean
end
declare class ProcessID
    function __tostring(self): string
end
declare class ChildProcess
    pid: ProcessID
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
export type stdprocess = {
    args: ArgsSpan,
    execute: (exe: string, ...string)->ExecutionFeedback,
    exists_in_path_environment: (what: string)->boolean,
    find_in_path_environment: (what: string)->string?,
    Child: ((exe: string, ...string)->ChildProcess)
        & ((pid: ProcessID)->ChildProcess),
    parse_args: (args: {string}) -> unknown,
}
export type ConsoleColor = {
        Black: 'Black0x00',
        Red: 'Red0x01',
        Green: 'Green0x02',
        Yellow: 'Yellow0x03',
        Blue: 'Blue0x04',
        Magenta: 'Magenta0x05',
        Cyan: 'Cyan0x06',
        White: 'White0x07',
        BrightBlack: 'BrightBlack0x08',
        BrightRed: 'BrightRed0x09',
        BrightGreen: 'BrightGreen0x0a',
        BrightYellow: 'BrightYellow0x0b',
        BrightBlue: 'BrightBlue0x0c',
        BrightMagenta: 'BrightMagenta0x0d',
        BrightCyan: 'BrightCyan0x0e',
        BrightWhite: 'BrightWhite0x0f',
}
export type stdio = {
        ConsoleColor: ConsoleColor,
}
export type stddatetime = {
    Date: (year: number, month: number, day: number) -> (),
}
export type stdhalia = {
        filesystem: stdfilesystem,
        math: stdmath,
        datetime: stddatetime,
        process: stdprocess,
        io: stdio,
        co_spawn: <Args...>((Args...)->(), Args...) -> thread,
        co_wait: (seconds: number)->(),
        terminate: (exit_code: number?)->(),
        Event: (<Params...>()->Event<Params...>),
}
declare std: stdhalia
