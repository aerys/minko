table.inject(premake.tools.gcc, 'cppflags.system.linux', {
	"-MMD", "-MP"
})

table.inject(premake.tools.gcc, 'cppflags.system.macosx', {
	"-MMD", "-MP"
})

table.inject(premake.tools.gcc, 'cxxflags.system.linux', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'cxxflags.system.macosx', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'tools.linux', {
	cxx = MINKO_HOME .. '/tool/lin/script/g++.sh',
})

table.inject(premake.tools.gcc, 'tools.macosx', {
	cxx = MINKO_HOME .. '/tool/mac/script/g++.sh',
})
