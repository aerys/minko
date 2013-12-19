minko = {}

if _ACTION == 'clean' then
	local prjs = solution().projects
	for i, prj in ipairs(prjs) do
    os.rmdir(prj.basedir .. "/bin")
    os.rmdir(prj.basedir .. "/obj")
	end
	os.rmdir(solution().basedir .. "/doc")
end
