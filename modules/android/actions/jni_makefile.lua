--
-- jni_makefile.lua
-- Generate a solution-level androidfile.
-- Copyright (c) 2012 Richard Swift and the Premake project
--

	function premake.jni_makefile(sln)
		_p('LOCAL_PATH := $(call my-dir)')
		for _,prj in ipairs(sln.projects) do
			_p('include $(LOCAL_PATH)/%s/Android.mk',prj.name)
		end
	end
