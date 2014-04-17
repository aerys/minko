-- Generate a solution-level androidfile.
--
-- originally from:
-- Copyright (c) 2012 Richard Swift and the Premake project

local jni = premake.extensions.jni
local project = premake.project

function jni.makefile(sln)
	_p('LOCAL_PATH := $(call my-dir)')
	for _,prj in ipairs(sln.projects) do
		_p('include %s/Android.mk', path.getrelative(sln.location, prj.location))
	end
	_p('')
end

function jni.default_makefile(sln)
	_p('include Application.mk')
	_p('include Android.mk')

	_p('all:')
	_p('\t${NDK_HOME}/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_TOOLCHAIN_VERSION=clang NDK_LOG=1 V=1')
	_p('')
	_p('clean:')
	_p('\t${NDK_HOME}/ndk-build clean NDK_APPLICATION_MK=`pwd`/Application.mk NDK_LOG=1 V=1')
	_p('')
end
