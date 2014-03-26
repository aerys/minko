PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	
	removeplatforms { "osx64", "ios", "android" }
	
	kind "StaticLib"
	language "C++"

	minko.plugin.enable("lua")
	
	files {
		"lib/cef3/include/**.hpp",
		"lib/cef3/include/**.h",
		"lib/cef3/include/**.cpp",
		"lib/cef3/include/**.c",
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs { "include" }

	-- Chromium Embedded Framework CPP wrapper (windows, linux)
	configuration {"windows32 or windows64 or linux32 or linux64"}
		includedirs { "lib/cef3" }
		files {
			"lib/cef3/libcef_dll/**.hpp",
			"lib/cef3/libcef_dll/**.hh",
			"lib/cef3/libcef_dll/**.h",
			"lib/cef3/libcef_dll/**.cpp",
			"lib/cef3/libcef_dll/**.cc",
			"lib/cef3/libcef_dll/**.c"
		}
		defines { 
			"CHROMIUM", 
			"V8_DEPRECATION_WARNINGS",
			"CHROMIUM_BUILD",
			"USING_CEF_SHARED",
			"USE_DEFAULT_RENDER_THEME=0",
			"USE_LIBJPEG_TURBO=1",
			"ENABLE_REMOTING=0",
			"ENABLE_WEBRTC=0",
			"ENABLE_EGLIMAGE=0",
			"ENABLE_TASK_MANAGER=0",
			"ENABLE_EXTENSIONS=0",
			"ENABLE_PLUGIN_INSTALLATION=0",
			"ENABLE_PLUGINS=0",
			"ENABLE_SESSION_SERVICE=0",
			"ENABLE_THEMES=0",
			"ENABLE_AUTOFILL_DIALOG=0",
			"ENABLE_BACKGROUND=0",
			"ENABLE_AUTOMATION=0",
			"ENABLE_GOOGLE_NOW=0",
			"ENABLE_FULL_PRINTING=0",
			"ENABLE_PRINTING=0",
			"ENABLE_SPELLCHECK=0",
			"ENABLE_CAPTIVE_PORTAL_DETECTION=0",
			"ENABLE_APP_LIST=0",
			"ENABLE_SETTINGS_APP=0",
			"ENABLE_MANAGED_USERS=0",
			"ENABLE_MDNS=1",
			"CLD_VERSION=2",
			"ICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_STATIC",
			"DYNAMIC_ANNOTATIONS_ENABLED=1",
			"WTF_USE_DYNAMIC_ANNOTATIONS=1",
			"__STDC_CONSTANT_MACROS",
			"__STDC_FORMAT_MACROS"
		}

	-- windows
	configuration { "windows32 or windows64" }
		defines {
			"NOMINMAX",
			"PSAPI_VERSION=1",
			"_CRT_RAND_S",
			"CERT_CHAIN_PARA_HAS_EXTRA_FIELDS",
			"_ATL_NO_OPENGL",
			"_SECURE_ATL",
			"TOOLKIT_VIEWS=1",
			"USE_AURA=1",
			"USE_ASH=1",
			"_CRT_SECURE_NO_DEPRECATE",
			"_SCL_SECURE_NO_DEPRECATE",
			"NTDDI_VERSION=0x06020000"
		}

	-- linux
	configuration { "linux32 or linux64" }
		buildoptions {"-pthread"}
		defines { 
			"USE_CAIRO=0",
			"USE_GLIB=0",
			"USE_NSS=1",
			"USE_X11=0",
			"GTK_DISABLE_SINGLE_INCLUDES=1",
			"USE_XI2_MT=2"
		}
		--fixme: find a way not to use absolute paths for wider compatibility
		includedirs {
			"lib/cef3",
			"/usr/include/gtk-2.0", 
			"/usr/include/glib-2.0", 
			"/usr/lib/i386-linux-gnu/glib-2.0/include/", 
			"/usr/include/cairo/", 
			"/usr/include/pango-1.0/",
			"/usr/lib/i386-linux-gnu/gtk-2.0/include/",
			"/usr/include/gdk-pixbuf-2.0/",
			"/usr/include/atk-1.0/"
		}

--[[	-- emscripten
	configuration { "html5" }
		includedirs { "" }]]--
