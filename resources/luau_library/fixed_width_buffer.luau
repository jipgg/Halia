--!strict
local width_32 = 4
local width_64 = 8
local width_16 = 2
local width_8 = 1
export type float = 'float'
export type double = 'double'
export type int8 = 'int8'
export type uint8 = 'uint8'
export type int16 = 'int16'
export type uint16 = 'uint16'
export type int32 = 'int32'
export type uint32 = 'uint32'
export type anybuffer = buffer
    & buffer_float
    & buffer_int8
    & buffer_uint8
    & buffer_double
    & buffer_uint16
    & buffer_int16
    & buffer_int32
    & buffer_uint32

export type fixed_width_type = float | double | int8 | uint8 | int16 | uint16 | int32 | uint32
local fixed_width = {float = width_32, double = width_64, int8 = width_8,
    uint8 = width_8, int16 = width_16, uint16 = width_16, int32 = width_32,
    uint32 = width_32
}
local postfix = {float = 'f32', double = 'f64', int8 = 'i8', uint8 = 'u8',
    int16 = 'i16', uint16 = 'u16', int32 =  'i32', uint32 = 'u32'
}
local fixed_width_buffer = {}

function fixed_width_buffer.create(fixed_type: fixed_width_type, size: number)
    return buffer.create(size * fixed_width[fixed_type])
end

function fixed_width_buffer.read(fixed: fixed_width_type, b: anybuffer, idx: number)
    return buffer['read'..postfix[fixed]](b, idx * fixed_width[fixed])
end
function fixed_width_buffer.write(fixed: fixed_width_type, b: anybuffer, idx: number, val: number)
    buffer['write'..postfix[fixed]](b, idx * fixed_width[fixed], val)
end

function fixed_width_buffer.len(fixed: fixed_width_type, b: anybuffer)
    return buffer.len(b) / fixed_width[fixed]
end

function fixed_width_buffer.copy(fixed: fixed_width_type, target: anybuffer, target_offset: number, source: anybuffer, source_offset: number?, count: number?)
    buffer.copy(target, target_offset * fixed_width[fixed], source, source_offset * fixed_width[fixed], count)
end

export type fixed_width_buffer = {
    create: ((float, size: number)->buffer_float)
        & ((double, size: number)->buffer_double)
        & ((int8, size: number)->buffer_int8)
        & ((uint8, size: number)->buffer_uint8)
        & ((int16, size: number)->buffer_int16)
        & ((uint16, size: number)->buffer_uint16)
        & ((int32, size: number)->buffer_int32)
        & ((uint32, size: number)->buffer_uint32),
    read: ((float, b: buffer_float, idx: number)->number)
        & ((double, b: buffer_double, idx: number)->number)
        & ((int8, b: buffer_int8, idx: number)->number)
        & ((uint8, b: buffer_uint8, idx: number)->number)
        & ((int16, b: buffer_int16, idx: number)->number)
        & ((uint16, b: buffer_uint16, idx: number)->number)
        & ((int32, b: buffer_int32, idx: number)->number)
        & ((uint32, b: buffer_uint32, idx: number)->number),
    write: ((float, b: buffer_float, idx: number, val: number)->number)
        & ((double, b: buffer_double, idx: number, val: number)->number)
        & ((int8, b: buffer_int8, idx: number, val: number)->number)
        & ((uint8, b: buffer_uint8, idx: number, val: number)->number)
        & ((int16, b: buffer_int16, idx: number, val: number)->number)
        & ((uint16, b: buffer_uint16, idx: number, val: number)->number)
        & ((int32, b: buffer_int32, idx: number, val: number)->number)
        & ((uint32, b: buffer_uint32, idx: number, val: number)->number),
    len: ((float, b: buffer_float)->number)
        & ((double, b: buffer_double)->number)
        & ((int8, b: buffer_int8)->number)
        & ((uint8, b: buffer_uint8)->number)
        & ((int16, b: buffer_int16)->number)
        & ((uint16, b: buffer_uint16)->number)
        & ((int32, b: buffer_int32)->number)
        & ((uint32, b: buffer_uint32)->number),
    copy: ((float, target: buffer_float, target_idx: number, source: buffer_float, source_idx: number?, count: number?)->())
        & ((double, target: buffer_double, target_idx: number, source: buffer_double, source_idx: number?, count: number?)->())
        & ((int8, target: buffer_int8, target_idx: number, source: buffer_int8, source_idx: number?, count: number?)->())
        & ((uint8, target: buffer_uint8, target_idx: number, source: buffer_uint8, source_idx: number?, count: number?)->())
        & ((int16, target: buffer_int16, target_idx: number, source: buffer_int16, source_idx: number?, count: number?)->())
        & ((uint16, target: buffer_uint16, target_idx: number, source: buffer_uint16, source_idx: number?, count: number?)->())
        & ((int32, target: buffer_int32, target_idx: number, source: buffer_int32, source_idx: number?, count: number?)->())
        & ((uint32, target: buffer_uint32, target_idx: number, source: buffer_uint32, source_idx: number?, count: number?)->())
}

return (fixed_width_buffer::any)::fixed_width_buffer
