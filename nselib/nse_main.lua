local coroutine = require "coroutine"
local yield = coroutine.yield
local wrap = coroutine.wrap

local package = require "package"
local string = require "string"
local gsub = string.gsub
local table = require "table"
local insert = table.insert

do 
  local function loader(lib)
	lib = lib:gsub("%.", "/")
	local name = "nselib/"..lib..".lua"

	return assert(loadfile(name))
  end

  insert(package.searchers, 1, loader)
end



local rules = ...;


print("nse_main.lua running.........\n")


local function get_chose_scripts(rules)
	local chosen_scripts = {}

	print("in get chose scripts\n")

	for i, rule in ipairs(rules) do
		print(rule)
	end


	local f = assert(loadfile("nselib/wsdd-app.nse"), "load wsdd-app.nse fail\n")

	local stauts, err = pcall(f)

	print(err)

	local status, err = pcall(action)

	print(err)

	return chosen_scripts

end

--load all chosen scripts
local chosen_scripts = get_chose_scripts(rules)


local function run(threads_iter, hosts)
	while threads_iter do
		local thread = threads_iter()

		
		print("saiyn:in run\n")

	end

end

local function main(hosts)


	print("in main:\n")



	local function threads_iter()

		yield(hosts)
	
	end


	run(wrap(threads_iter), hosts)

end


local f = assert(loadfile("nselib/wsdd-app.nse"), "load wsdd-app.nse fail\n")


return main


