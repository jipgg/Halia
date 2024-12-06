local args = std.process.args

print(args:at(0))
print(args.size)
for i, v in args:totable() do
    print(`{i} = {v}`)
end
local params = args:parse_params()
print(`printin params`)
for i, v in params do
    print(`{i} = {v}`)
end
print('printing flags')
for i, v in args:parse_flags() do
    print(`{i} = {v}`)
end
