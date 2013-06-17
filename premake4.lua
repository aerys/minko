solution "minko"
   configurations { "Debug", "Release" }

include 'examples'
include 'framework'

newaction {
	trigger = "copyDLLs",
	description = "Copy the required DLLs into the app. output folder",
	execute = function()
	  os.mkdir(project(1).basedir .. "/bin/release")
	  os.mkdir(project(1).basedir .. "/bin/debug")

	  for _, file in ipairs(os.matchfiles(project(2).basedir .. "/lib/win32/bin/*.dll")) do
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