local args = std.process.args
args:for_each(function(arg: string)
    print(`{arg} each`)
end)
print(args.size)
for i, v in args:to_table() do
    print(`{i} = {v}`)
end
std.co_wait(5)
local params = args:parse_params()
print(`printing params`)
for i, v in params do
    print(`{i} = {v}`)
end
print('printing flags')
for i, v in args:parse_flags() do
    print(`{i} = {v}`)
end
local exit_code, out, err = std.process.run('cmake --version', 'stdout | stderr')
print(`exit code {exit_code}`)
