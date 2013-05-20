solution "minko"
   configurations { "Debug", "Release" }

include 'examples'
include 'framework'

newaction {
	trigger = "copyDLLs",
	description = "Copy the required DLLs into the app. output folder",
	execute = function()
	  os.mkdir(project(1).basedir .. "/bin")
	  os.copyfile(
		project(2).basedir .. "/lib/win32/bin/freeglut.dll",
		project(1).basedir .. "/bin/freeglut.dll"
	  )
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