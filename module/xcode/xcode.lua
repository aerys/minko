--
-- _xcode.lua
-- Define the Apple XCode action and support functions.
-- Copyright (c) 2009 Jason Perkins and the Premake project
--




	premake.xcode = { }
	local api = premake.api

---
---
	api.register {
		name = "xcodebuildsettings",
		scope = "config",
		kind = "key-array",
	}
	
	api.register {
		name = "xcodebuildresources",
		scope = "config",
		kind = "list",
	}	
	
---
---

	dofile("module/xcode/xcode_common.lua")
	dofile("module/xcode/xcode_workspace.lua")
	dofile("module/xcode/xcode_project.lua")
	
	-- I had to separate solution generation into 2 actions in order to have a
	-- single platform to support per action. This is needed because Xcode is
	-- unable to do conditional compilation (only compile files for a specific
	-- platform).

	newaction 
	{
		trigger         = "xcode-ios",
		shortname       = "Xcode (iOS)",
		description     = "Generate Apple Xcode project files for iOS",
		os              = "macosx",

		valid_kinds     = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib", "Makefile", "None" },
		
		valid_languages = { "C", "C++" },
		
		valid_tools     = 
		{
			cc     = { "gcc" , "clang"},
		},

		valid_platforms = 
		{
			ios		= "ios"
		},
		
		default_platform = "ios",
		
		onsolution = function(sln)
			premake.generate(sln, ".xcworkspace/contents.xcworkspacedata", premake.xcode.workspace_generate)
		end,
		
		onproject = function(prj)
			premake.generate(prj, ".xcodeproj/project.pbxproj", premake.xcode.project)
		end,
		
		oncleanproject = function(prj)
		end,

		oncleansolution = function(sln)
		end,
		
		oncleantarget   = function()
		end, 
		
		oncheckproject = function(prj)
			-- Xcode can't mix target kinds within a project
			local last
			for cfg in project.eachconfig(prj) do
				if last and last ~= cfg.kind then
					error("Project '" .. prj.name .. "' uses more than one target kind; not supported by Xcode", 0)
				end
				last = cfg.kind
			end
		end,
	}		

	newaction 
	{
		trigger         = "xcode-osx",
		shortname       = "Xcode (OS X)",
		description     = "Generate Apple Xcode project files for OS X",
		os              = "macosx",

		valid_kinds     = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib", "Makefile", "None" },
		
		valid_languages = { "C", "C++" },
		
		valid_tools     = 
		{
			cc     = { "gcc" , "clang"},
		},

		valid_platforms = 
		{
			osx64	= "osx64"
		},
		
		default_platform = "osx64",
		
		onsolution = function(sln)
			premake.generate(sln, ".xcworkspace/contents.xcworkspacedata", premake.xcode.workspace_generate)
		end,
		
		onproject = function(prj)
			premake.generate(prj, ".xcodeproj/project.pbxproj", premake.xcode.project)
		end,
		
		oncleanproject = function(prj)
		end,

		oncleansolution = function(sln)
		end,
		
		oncleantarget   = function()
		end, 
		
		oncheckproject = function(prj)
			-- Xcode can't mix target kinds within a project
			local last
			for cfg in project.eachconfig(prj) do
				if last and last ~= cfg.kind then
					error("Project '" .. prj.name .. "' uses more than one target kind; not supported by Xcode", 0)
				end
				last = cfg.kind
			end
		end,
	}	
	
	newoption
	{
		trigger     = "modules",
		value       = "path",
		description = "Search for additional scripts on the given path"
	}
