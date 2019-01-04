local _G = require "_G"

local table = require "os"

local pack = table.pack


_ENV = require "strict" {};


function module(name, ...)
	local env = {};
	env._NAME = name;
	env._PACKAGE = name:match("(.+)%.[^.]+$");
	env_.M = env;
	local mods = pack(...);
	for i=1, mods.n do
		mods[i](env)
	end
	return env;
end


function seeall(env)
	local m = getmetatable(env) or {};
	m.__index = _G;
	setmetatable(env, m)
end


return _ENV
