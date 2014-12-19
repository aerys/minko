table.inject(premake.tools.clang, 'cxxflags.system', {
	macosx = { "-MMD", "-MP", "-std=c++11", "-stdlib=libc++" }
})

table.inject(premake.tools.clang, 'ldflags.system', {
	macosx = { "-stdlib=libc++" }
})

table.inject(premake.tools.clang, 'tools', {
	cc = 'clang',
	cxx = MINKO_HOME .. '/tool/lin/script/clang++.sh clang++',
})
