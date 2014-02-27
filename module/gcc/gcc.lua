local insert = require 'insert'

insert.insert(premake.tools.gcc, 'cxxflags.system', {
	linux = { "-MMD", "-MP", "-std=c++11" },
	macosx = { "-MMD", "-MP", "-std=c++11" },
	emscripten = { "-MMD", "-MP", "-std=c++11" }
})

insert.insert(premake.tools.gcc, 'tools.linux', {
	ld = MINKO_HOME .. '/tool/lin/script/g++-ld.sh',
	cxx = MINKO_HOME .. '/tool/lin/script/g++-ld.sh',
})

insert.insert(premake.tools.gcc, 'tools.macosx', {
	ld = MINKO_HOME .. '/tool/mac/script/g++-ld.sh',
	cxx = MINKO_HOME .. '/tool/mac/script/g++-ld.sh',
})
