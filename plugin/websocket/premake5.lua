PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include",
		"src"
	}

	excludes {
		"src/minko/net/EmscriptenWebSocketImpl.hpp",
		"src/minko/net/EmscriptenWebSocketImpl.cpp"
	}

	configuration { "not html5" }
		-- websocket++
		files {
			"lib/websocketpp/**.hpp",
			"lib/websocketpp/**.hpp",
			"lib/asio/include/**.hpp",
			"lib/openssl/include/**.h"
		}
		includedirs {
			"lib/websocketpp",
			"lib/asio/include",
			"lib/openssl/include"
		}
		defines {
			"ASIO_STANDALONE",
			"_WEBSOCKETPP_CPP11_INTERNAL_",
		}

	configuration { "linux32 or linux64" }
		links {
			"ssl",
			"crypto"
		}

	configuration { "windows32" }
		defines {
			"_WIN32_WINNT=0x0501"
		}
		links {
			"lib/openssl/lib/win32/ssleay32",
			"lib/openssl/lib/win32/libeay32"
		}

	configuration { "html5" }
		excludes {
			"src/minko/net/NativeWebSocketImpl.hpp",
			"src/minko/net/NativeWebSocketImpl.cpp"
		}
		files {
			"src/minko/net/EmscriptenWebSocketImpl.hpp",
			"src/minko/net/EmscriptenWebSocketImpl.cpp"
		}
