minko.package = {
    _path = {}
}

-- supported actions are 'copy', 'link', 'embed', 'optimize', 'minify'

-- minko.package.assets {
--     ['effect/*.effect'] = { 'minify', 'copy' }
-- }

-- to simulate legacy behavior:
-- minko.package.assets {
--     ['*'] = { 'copy', 'embed' }
-- }

minko.package.assetdirs = function(directories)
    if type(directories) == 'string' then
        directories = { directories }
    end

    assert(type(directories) == 'table', '`minko.package.assetdirs` expects a string or an array of strings')

    for i = 1, #directories do
        assert(type(directories[i]) == 'string', '`minko.package.assetdirs` expects a string or an array of strings')
        minko.package._path[#minko.package._path + 1] = directories[i]
    end
end

minko.package.assets = function(rules)
    assert(type(rules) == 'table', '`minko.package.assets` expects an array of rules')

    for rule, actions in pairs(rules) do
        assert(type(rule) == 'string', 'rules expect a globbing string (["*.ext"] = { "action", ... })')
        assert(type(actions) == 'table', 'rules expect an action list (["*.ext"] = { "action", ... })')

        local inputfiles = {}

        for i = 1, #minko.package._path do
            local pattern = path.join(minko.package._path[i], rule)
            local files = {}

            if os.isfile(pattern) or os.isdir(pattern) then
                files = { pattern }
            elseif string.find(pattern, '*') then
                files = os.matchfiles(pattern)
            end

            for j = 1, #files do
                print(files[j])
                if not inputfiles[files[j]] then
                    -- print("file " .. files[j])
                    inputfiles[files[j]] = {
                        file = files[j],
                        path = minko.package._path[i],
                        rule = rule
                    }
                end
            end
        end

        for k, v in pairs(inputfiles) do
            local path_ = v.path
            local input = v.file
            local output = nil

            -- assert(not os.isdir(input), 'rules do not expect a directory')

            for j = 1, #actions do
                local action = actions[j]
                local options = nil

                assert(type(action) == 'string' or type(action) == 'table', 'actions should be a string ("copy") or a table ({ action = "copy", options = {...} })')

                if type(action) == 'table' then
                    options = action.options
                    action = action.action
                end

                if minko.action[action] == nil then
                    error(color.fg.red .. 'unknown action `' .. action .. '`' .. color.reset)
                else
                    input = output or input

                    local haystack = path.getabsolute(input)
                    local needle = path.getabsolute(path.getdirectory(path_))
                    -- assert(string.find(haystack, needle) ~= nil, 'input file "' .. haystack .. '" is not in path "' .. needle .. '"')

                    output = string.sub(haystack, string.len(needle .. '/') + 1)

                    local command = minko.action[action](input, output)

                    -- print("action:\t" .. action)
                    -- print("path:\t" .. path_)
                    -- print("input:\t" .. input)
                    -- print("output:\t" .. output)
                    -- print("cmd:\t" .. command)

                    prelinkcommands {
                        command
                    }
                end
            end
        end
    end
end
