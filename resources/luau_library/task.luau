local task = {}

function task.spawn(fn, ...)
    -- Start a new coroutine and execute the function within it
    local co = coroutine.create(fn)
    local success, err = coroutine.resume(co, ...)
    if not success then
        error(err)
    end
end

function task.wait(count)
    -- Ensure that `task.wait` is called within a coroutine
    local co = coroutine.running()
    if not co then
        error("task.wait must be called within a coroutine")
    end
    local accumulated = 0
    local id
    id = engine.updating:connect(function(delta_seconds)
        accumulated = accumulated + delta_seconds
        if accumulated >= count then
            engine.updating:disconnect(id)
            coroutine.resume(co)
        end
    end)
    coroutine.yield()
end

return task
