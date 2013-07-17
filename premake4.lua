solution "minko"
   configurations { "debug", "release" }

-- examples
include 'examples'

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
include 'plugins/webgl'

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

newaction {
	trigger = "fixMakefilesForEmscripten",
	description = "Fix Makefiles for Emscripten compatibility.",
	execute = function()
		os.execute('scripts/fix-makefiles.sh')
	end
}
-- FIXME: Move to scripts/ with dofile("scripts/toolchain.lua")

function newplatform(platform)
	local name = platform.name
	local description = platform.description
 
	-- Register new platform
	premake.platforms[name] = {
		cfgsuffix = "_"..name,
		iscrosscompiler = true
	}
 
	-- Allow use of new platform in --platfroms
	table.insert(premake.option.list["platform"].allowed, { name, description })
	table.insert(premake.fields.platforms.allowed, name)
 
	-- Add compiler support
	-- gcc
	premake.gcc.platforms[name] = platform.gcc
	--other compilers (?)
end
 
newplatform {
	name = "emscripten",
	description = "Emscripten C++ to JS toolchain",
	gcc = {
		cc = "emcc",
		cxx = "em++",
		ar = "emar",
		cppflags = "-DEMSCRIPTEN"
		-- premake.gcc.prebuildcommands = { 'scripts/fix-makefiles.sh' }
	}
}

newplatform {
	name = "clang",
	description = "Clang",
	gcc = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar",
		cppflags = "-MMD "
	}
}


if _OPTIONS.platform then
	print("Selected target platform: " .. _OPTIONS["platform"])
    -- overwrite the native platform with the options::platform
    premake.gcc.platforms['Native'] = premake.gcc.platforms[_OPTIONS.platform]
end
