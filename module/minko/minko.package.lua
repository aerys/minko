minko.package = {}

dofile('minko.package.stats.lua')

minko.package.get_copy = function(file)
    minko.package.stats.addCopy()
    return minko.action.copy(file)
end

minko.package.get_convert = function(file)
    return minko.package.convert(file)
end

minko.package.copy = function(files)
    for i = 1, #files do
        print(files[i] .. ' will be copied')
        prelinkcommands { minko.package.get_copy(files[i]) }
    end
end

minko.package.embed = function(data)
    local name, match = path.getname(os.getcwd()), nil

    print("Building " .. name .. " package")
    for key, val in pairs(data) do
        if type(val) == "table" then
            match = os.matchfiles(path.getabsolute(os.getcwd()) .. '/asset/' .. key)
            if next(match) ~= nil then
                minko.package[val.action](match)
            end
        end
    end
    minko.package.stats.addPackageNumber()
    printPrelinkStatsLocal()
    printPrelinkStatsGlobal()
end

minko.package.convert = function(files)
    for i = 1, #files do
        if minko.package.checkIfCompatible(files[i]) == true then
            print(files[i] .. ' will be converted')
            prelinkcommands { minko.package.prelinkConvert(files[i]) }
            minko.package.stats.addConvert()
        else
            print('Incompatible extension on ' .. files[i] .. ' file and cannot be converted')
        end
    end
end

minko.package.autodetectExtension = function (filename)
        local reversed = string.reverse(filename)
        local s, e = string.find(reversed, ".")
        local extension = string.reverse(string.sub(reversed, 1, s + 2))
        return extension
    end

minko.package.checkIfCompatible = function (filename)
    minko.package.compatibleExtensions = {}
    minko.package.compatibleExtensions[1] = "scene"
    minko.package.compatibleExtensions[2] = "dae"
    minko.package.compatibleExtensions[3] = "obj"
    minko.package.compatibleExtensions[4] = "fbx"
    minko.package.compatibleExtensions[5] = "3ds"
    local i = 1
    while (i < 6) do
        if minko.package.autodetectExtension(filename) == minko.package.compatibleExtensions[i] then
            return true
        end
        i = i + 1
    end
    return false
end

minko.package.prelinkConvert = function(filename)
    local convert_binary_name = "minko-scene-converter"
    local extension = minko.package.autodetectExtension(filename)
    local filename_length = string.len(filename)
    local filename_cp = filename
    local extension_length = string.len(extension)
    local extension_postion = filename_length - extension_length
    local input_f = filename
    local output_f = string.sub( filename_cp, 1 ,extension_postion )
    local output_f = output_f .. "scene"
    local output = output_f
    local input = input_f
    if os.is('windows') then
        if os.is64bit() then
            binary_convert = MINKO_HOME .. '/tool/win/bin/' .. convert_binary_name .. ".exe"
        else
            binary_convert = MINKO_HOME .. '/tool/win/bin/' .. convert_binary_name .. ".exe"
        end
    elseif os.is("macosx") then
        binary_convert = MINKO_HOME .. '/tool/mac/bin/x86_64/' .. convert_binary_name --no full name yet for macosx
    else
        if os.is64bit() then
            binary_convert = MINKO_HOME .. '/tool/lin/bin/x86_64/' .. convert_binary_name .. "-lin"
        else
            binary_convert = MINKO_HOME .. '/tool/lin/bin/i686/' .. convert_binary_name .. "-lin"
        end
    end
    local to_return = '"' .. binary_convert .. '" -i "' .. input .. '" -o "' .. output .. '"'
    if os.is('windows') then
        to_return = string.gsub(to_return, "/", "\\")
    end
    return to_return
end
