local coroutine = require "coroutine"
local yield = coroutine.yield
local wrap = coroutine.wrap





local rules = ...;



local function get_chose_scripts(rules)
	local chosen_scripts = {}

	return chosen_scripts

end

--load all chosen scripts
local chosen_scripts = get_chose_scripts(rules)


local function run(threads_iter, hosts)
	while threads_iter do
		local thread = threads_iter()
		print(thread)
	end



local function main(hosts)

	local function threads_iter()
		for j, host in ipairs(hosts) do
			yield(host)
		end
	end


	run(wrap(threads_iter), hosts)

end


return main


