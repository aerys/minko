function os.iscygwin()
	return string.startswith(os.getenv('OSTYPE'), 'CYGWIN')
end

function path.cygpath(filepath)
	filepath = path.translate(filepath, '/')

	if os.iscygwin() then
		local t = explode(':', filepath)
		if table.getn(t) == 2 then
			filepath = '/cygdrive/' .. string.lower(t[1]) .. string.gsub(t[2], '\\', '/')
		end
	end

	return filepath
end
