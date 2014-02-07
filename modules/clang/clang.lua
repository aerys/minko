local insert = require 'insert'

insert.insert(premake.tools.clang, 'cxxflags.system', {
	macosx = { "-MMD", "-MP", "-std=c++11", "-stdlib=libc++" }
})

insert.insert(premake.tools.clang, 'ldflags.system', {
	macosx = { "-stdlib=libc++" }
})
