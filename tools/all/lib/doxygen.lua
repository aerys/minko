if _ACTION == 'clean' then
	os.rmdir(solution().basedir .. "/doc")
end

newaction {
	trigger = "doxygen",
	description = "Create developer reference.",
	execute = function()
		os.execute("doxygen Doxyfile")
	end
}