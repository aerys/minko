minko.action = {}

minko.action.fail = function()
	return 'source ' .. minko.sdk.path('/tools/lin/bin/fail.sh') .. ' ${TARGET}'
end

minko.action.copy = function(sourcePath)
	if os.is('windows') then
		return 'xcopy /y /i /e "' .. path.translate(sourcePath) .. '" "$(TargetDir)"'
	else
		return 'cp -R "' .. sourcePath .. '" ${TARGETDIR}'
	end
end

minko.action.clean = function()
	-- if not os.isfile("sdk.lua") then
	-- 	error("cannot clean from outside the Minko SDK")
	-- end

	-- for _, dir1 in ipairs { "framework", "plugins", "tests", "examples" } do
	-- 	for _, dir2 in ipairs { "bin", "obj" } do
	-- 		local dirs = os.matchdirs(dir1 .. "/**/" .. dir2)
	-- 		for _, dir in ipairs(dirs) do
	-- 			os.rmdir(dir)
	-- 		end
	-- 	end
	-- end

	-- os.rmdir("doc")

	os.execute("git clean -X -f")
end
