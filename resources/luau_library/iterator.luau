local lib = {}

--- reverses an array
function lib.ireverse<T>(t: {T})
    local i = #t + 1
    return function(): T?
        i -= 1
        if i > 0 then
            return t[i]
        else
            return nil
        end
    end
end

function lib.ifilter<T>(t: {T}, pred: (e: T) -> boolean)
    local i = 0
    return function(): T?
        i += 1
        local v = t[i]
        while i <= #t and not pred(v) do
            i += 1
            v = t[i]
        end
        if i <= #t then
            return v
        else
            return nil
        end
    end
end
function lib.filter<T>(t: { [any]: T }, pred: (e: T) -> boolean)
    local key, value
    return function(): T?
        repeat
            key, value = next(t, key)
        until key == nil or pred(value)
        if key ~= nil then
            return value
        else
            return nil
        end
    end
end

function lib.elements<T>(t: {[any]: T})
    local key, value
    return function(): T?
        key, value = next(t, key)
        if key ~= nil then
            return value
        else
            return nil
        end
    end
end
function lib.imap<T, U>(t: {T}, u: {U})
    local i = 0
    return function(): (T?, U?)
        i += 1
        local tv = t[i]
        local uv = u[i]
        if i <= #t or i <= #u then
            return tv, uv
        else
            return nil, nil
        end
    end
end
lib.

return lib
