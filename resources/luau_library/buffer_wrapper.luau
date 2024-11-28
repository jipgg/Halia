--!strict
export type fixed_width_type = 'i8'|'i16'|'i32'|'u8'|'u16'|'u32'|'f32'|'f64'
export type public = {
    capacity: number,
    size: number,
    data: buffer,
    sizeof: (public, ty: fixed_width_type) -> number,
    push: (public, ty: fixed_width_type, ...number) -> (),
    pop: (public, ty: fixed_width_type, count: number) -> (),
    read: (public, ty: fixed_width_type, offset: number) -> (),
}

local buffer_wrapper = {}
buffer_wrapper.__index = buffer_wrapper
function buffer_wrapper.ctor(bytesize: number)
    return setmetatable({
        capacity = bytesize,
        size = 0,
        data = buffer.create(bytesize)
    }, buffer_wrapper)
end
export type self = typeof(buffer_wrapper.ctor(1))
type type_switch<Ts...> = {
    i8: (Ts...)->(),
    i16: (Ts...)->(),
    i32: (Ts...)->(),
    u8: (Ts...)->(),
    u16: (Ts...)->(),
    u32: (Ts...)->(),
    f32: (Ts...)->(),
    f64: (Ts...)->(),
}
type type_map<T> = {
    i8: T,
    i16: T,
    i32: T,
    u8: T,
    u16: T,
    u32: T,
    f32: T,
    f64: T,
}
local bytesizes: type_map<number> = {i8=1, i16=2, i32=4, u8=1, u16=2, u32=4, f32=4, f64=8}
function buffer_wrapper:pop(t: fixed_width_type, count: number)
    (self::self).size -= bytesizes[t] * count
end
local switch_read: type_switch<buffer, number> = {
    i8=function(b, off) buffer.readi8(b, off) end,
    i16=function(b, off) buffer.readi16(b, off) end,
    i32=function(b, off) buffer.readi32(b, off) end,
    u8=function(b, off) buffer.readu8(b, off) end,
    u16=function(b, off) buffer.readu16(b, off) end,
    u32=function(b, off) buffer.readu32(b, off) end,
    f32=function(b, off) buffer.readf32(b, off) end,
    f64=function(b, off) buffer.readf64(b, off) end,
}
function buffer_wrapper:read(t: fixed_width_type, offset: number)
    switch_read[t](self.data, offset)
end
function buffer_wrapper:sizeof(t: fixed_width_type)
    return bytesizes[t]
end
local pushImpl: type_switch<...any> = {
    i8 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    end,
    i16 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    end,
    i32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    u8 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    end,
    u16 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    end,
    u32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    f32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writef32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    f64 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writef64(self.data, self.size, select(i, ...))
            self.size += 8
        end
    end,
}

function buffer_wrapper:push(t: fixed_width_type, ...: number)
    pushImpl[t](self, ...)
end

return (buffer_wrapper.ctor :: unknown) :: (bytesize: number) -> public

