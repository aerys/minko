minko.action = {}

minko.action.fail = function()
	return 'source ' .. minko.sdk.path('/tools/lin/bin/fail.sh') .. ' ${TARGET}'
end
