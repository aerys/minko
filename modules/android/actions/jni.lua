--
-- _jni.lua
-- Define the makefile action(s).
-- Copyright (c) 2012 Richard Swift and the Premake project
--

	_jni = { }
	premake.jni = { }
	local jni = premake.jni

--
-- Escape a string so it can be written to a makefile.
--

	function _jni.esc(value)
		local result
		if (type(value) == "table") then
			result = { }
			for _,v in ipairs(value) do
				table.insert(result, _MAKE.esc(v))
			end
			return result
		else
			-- handle simple replacements
			result = value:gsub("\\", "\\\\")
			result = result:gsub(" ", "\\ ")
			result = result:gsub("%(", "\\%(")
			result = result:gsub("%)", "\\%)")
			
			-- leave $(...) shell replacement sequences alone
			result = result:gsub("$\\%((.-)\\%)", "$%(%1%)")
			return result
		end
	end
	

	function _jni.getmakefilename(this)
		-- how many projects/solutions use this location?
		local count = 0
		for sln in premake.solution.each() do
			for _,prj in ipairs(sln.projects) do
				if (prj.location == this.location) then count = count + 1 end
			end
		end
		
		if (count == 1) then
			return "Android.mk"
		else
			return this.name .. "\\Android.mk"
		end
	end

--
-- Register the "jni" action
--

	newaction {
		trigger         = "jni",
		shortname       = "jni ndk-build",
		description     = "Generate ndk-build makefiles for android",
	
		valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib" },
		
		valid_languages = { "C", "C++" },
		
		valid_platforms = { "jni" },
		
		valid_tools     = {
			cc     = { "gcc" },
		},
		
		onsolution = function(sln)
			premake.generate(sln, "Application.mk", premake.jni_solution)
			premake.generate(sln, "Android.mk", premake.jni_makefile)
		end,
		
		onproject = function(prj)
			premake.generate(prj, _jni.getmakefilename(prj), premake.jni_cpp)
		end,
		
		oncleansolution = function(sln)
			premake.clean.file(sln, "Application.mk")
		end,
		
		oncleanproject = function(prj)
			premake.clean.file(prj, _jni.getmakefilename(prj))
		end
	}
