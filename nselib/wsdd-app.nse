local wsdd = require "wsdd"

action = function(host, port)

	print("in wsdd action")

	local helper = wsdd.Helper:new(host, port)

	helper:setTimeout(5000)

	local status, result = helper['discoverDevices'](helper)
	

	print(result)
end



