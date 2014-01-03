minko = {}

function minko.fail()
	return 'source ' .. minko.sdk.path('/tools/all/bin/fail.sh') .. ' ${TARGET}'
end
