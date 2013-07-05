solution "minko"
   configurations { "Debug", "Release" }

-- examples
include 'examples'

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
<<<<<<< HEAD
include 'plugins/bullet'
include 'plugins/mk'
=======
>>>>>>> 8476c1b... Remove mk from premake file in master branch

newaction {
	trigger = "copyDLLs",
	description = "Copy the required DLLs into the app. output folder",
	execute = function()
	  os.mkdir(project(1).basedir .. "/bin/release")
	  os.mkdir(project(1).basedir .. "/bin/debug")

	  for _, file in ipairs(os.matchfiles("**.dll")) do
		os.copyfile(
			file,
			project(1).basedir .. "/bin/release/" .. path.getname(file)
		)
		os.copyfile(
			file,
			project(1).basedir .. "/bin/debug/" .. path.getname(file)
		)
	  end
	end
}

newaction {
	trigger = "removeDLLs",
	description = "Remove the DLLs copied by copyDLLs",
	execute = function()
	  os.rmdir(project(1).basedir .. "/bin")
	end
}

newaction {
	trigger = "cleanVS",
	description = "Remove additional files added by VS",
	execute = function()
		os.rmdir("minko.sdf")
		os.rmdir("minko.v11.suo")
		os.rmdir("minko.suo")
	end
}

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
