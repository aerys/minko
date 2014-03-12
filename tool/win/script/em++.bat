@echo off
@setlocal enabledelayedexpansion

if not defined EMSCRIPTEN (
	echo Environment variable EMSCRIPTEN is not defined.
	pause
	exit /b 1
)

set BIN="%EMSCRIPTEN%\em++.bat"
set ARGS=
set STATIC_LIBS=
set SHARED_LIBS=

set START_GROUP=-Wl,--start-group
set END_GROUP=-Wl,--end-group

for %%A in (%*) do (
	set ARG=%%A
	if "!ARG:~-2!" == ".a" (
		set STATIC_LIBS=!STATIC_LIBS! !ARG!
	) else (
		if "!ARG:~0,2!" == "-l" (
			set SHARED_LIBS=!SHARED_LIBS! !ARG!
		) else (
			if not "!ARG!" == "-s" (
				set ARGS=!ARGS! !ARG!
			)
		)
	)
)

set EMPTY=1
if not "%STATIC_LIBS%" == "" set EMPTY=0
if not "%SHARED_LIBS%" == "" set EMPTY=0

if %EMPTY% == 1 (
	@if %verbose% == 1 echo on
	%BIN% %ARGS%
) else (
	@if %verbose% == 1 echo on
	%BIN% %ARGS% %START_GROUP% %STATIC_LIBS% %STATIC_LIBS% %SHARED_LIBS% %END_GROUP%
)

@set error=%errorlevel%

@echo off

echo error is %error%
if %error% neq 0 exit /b %error%
