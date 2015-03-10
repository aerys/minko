--
-- Extend the existing exporters with support for Visual Studio 2015.
--

	local vstudio = premake.vstudio
	local cs2005 = vstudio.cs2005
	local vc2010 = vstudio.vc2010
---
-- Define the Visual Studio 2015 export action.
---

	newaction {
		-- Metadata for the command line and help system

		trigger     = "vs2015ctp",
		shortname   = "Visual Studio 2015 CTP",
		description = "Generate Visual Studio 2015 project files",

		-- Visual Studio always uses Windows path and naming conventions

		os = "windows",

		-- The capabilities of this action

		valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib", "Makefile", "None" },
		valid_languages = { "C", "C++", "C#" },
		valid_tools     = {
			cc     = { "msc"   },
			dotnet = { "msnet" },
		},

		-- Solution and project generation logic

		onsolution = vstudio.vs2005.generateSolution,
		onproject  = vstudio.vs2010.generateProject,

		oncleansolution = vstudio.cleanSolution,
		oncleanproject  = vstudio.cleanProject,
		oncleantarget   = vstudio.cleanTarget,

		-- This stuff is specific to the Visual Studio exporters

		vstudio = {
			solutionVersion = "14",
			versionName     = "2015",
			targetFramework = "4.6",
			toolsVersion    = "14.0",
		}
	}


---
-- Add new elements to the configuration properties block of C++ projects.
---

	premake.override(vc2010, "platformToolset", function(orig, cfg)
		if _ACTION == "vs2015ctp" then
			_p(2,'<PlatformToolset>v140</PlatformToolset>')
		elseif _ACTION > "vs2013" then
			_p(2,'<PlatformToolset>v120</PlatformToolset>')
		else
			orig(cfg)
		end
	end)
