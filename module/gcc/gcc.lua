local insert = require 'insert'

insert.insert(premake.tools.gcc, 'cxxflags.system', {
	linux = { "-MMD", "-MP", "-std=c++11" },
	macosx = { "-MMD", "-MP", "-std=c++11" },
	emscripten = { "-MMD", "-MP", '"-std=c++11"' } -- note: double-quoting is needed by cmd.exe
})

insert.insert(premake.tools.gcc, 'tools.linux', {
	cxx = MINKO_HOME .. '/tool/lin/script/g++.sh',
})

insert.insert(premake.tools.gcc, 'tools.macosx', {
	cxx = MINKO_HOME .. '/tool/mac/script/g++.sh',
})
