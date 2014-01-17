minko.vs = {}

if _ACTION == 'clean' then
	os.remove(solution().basedir .. "/minko.sdf")
	os.remove(solution().basedir .. "/minko.v11.suo")
	os.remove(solution().basedir .. "/minko.suo")
end

newaction {
	trigger = "installTemplates",
	description = "Copy the project/item templates into the user's directory.",
	execute = function()
		userprofile = path.translate(os.getenv("userprofile"), "/")
		outputDirs = {
			userprofile .. "/documents/Visual Studio 2010/Templates/ItemTemplates",
			userprofile .. "/documents/Visual Studio 2012/Templates/ItemTemplates",
			userprofile .. "/documents/Visual Studio 2013/Templates/ItemTemplates"
		}
		for _, templateDir in ipairs(os.matchdirs("templates/vs/*")) do
			for _, outputDir in ipairs(outputDirs) do
				outputDir = outputDir .. "/" .. path.getname(templateDir)
				os.mkdir(outputDir)
				for _, templateFile in ipairs(os.matchfiles(templateDir .. "/*")) do
					success , errormsg = os.copyfile(path.getabsolute(templateFile), outputDir .. "/" .. path.getname(templateFile))
					if (success == nil) then
						print(errormsg)
					end
				end
			end
		end
	end
}

minko.vs.getdllscopycommand = function(sourceDir)
	return 'xcopy /y "' .. path.translate(sourceDir, '\\') .. '\\*.dll" "$(TargetDir)"'
end
