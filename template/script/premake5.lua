if MINKO_HOME == nil then
	if os.getenv('MINKO_HOME') then
		MINKO_HOME = os.getenv('MINKO_HOME');
	else
		error('You must define the environment variable MINKO_HOME.')
	end
end

if not os.isfile(MINKO_HOME .. '/sdk.lua') then
	error('MINKO_HOME does not point to a valid Minko SDK.')
end

dofile(MINKO_HOME .. "/sdk.lua")
