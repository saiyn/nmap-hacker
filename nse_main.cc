#include "nse_utility.h"
#include "Target.h"
#include "util.h"
#include "nse_debug.h"


#define NSE_PARALLELISM   "NSE_PARALLELISM"
#define NSE_CURRENT_HOSTS "NSE_CURRENT_HOSTS"
#define NSE_MAIN	  "NSE_MAIN"


#define MAXPATHLEN (2048)


#if 0

static void set_nmap_libraries(lua_State *L)
{
	static const luaL_Reg libs[] = {
		{"nmap", luaopen_namp},

#ifdef HAVE_OPENSSL
		{"openssl", luaopen_openssl},
#endif
		{NULL, NULL}

	};


	for(int i = 0; libs[i].name; i++){
		luaL_requiref(L, libs[i].name, libs[i].func, 1);
		lua_pop(L, 1);
	}

}

static void open_cnse(lua_State *L)
{
	static const luaL_Reg nse[] = {
		{"log_write", l_log_write},
		{NULL, NULL}
	};

	luaL_newlib(L, bse);

	nseU_setbfield(L, -1, "default", o.script);
	nseU_setsfield(L, -1, "scriptargs", o.scriptargs);
}

#endif

static int init_main(lua_State *L)
{
	char path[MAXPATHLEN] = {0};
	std::vector<std::string> *rules = (std::vector<std::string> *)lua_touserdata(L, 1);


	luaL_openlibs(L);
	//set_nmap_libraries(L);

	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, NSE_CURRENT_HOSTS);

	//if(nmap_fetchfile(path, sizeof(path), "nse_main.lua") != 1)
	//	luaL_error(L, "could not load nse_main.lua:%s", lua_tostring(L, -1));

	sprintf(path, "nselib/nse_main.lua");

	if(luaL_loadfile(L, path) != 0)
		printf("could not load nse_main.lua:%s\n", lua_tostring(L,-1));
		//luaL_error(L, "could not load nse_main.lua:%s", lua_tostring(L, -1));


	std::cout << "saiyn: load " << path << "success" << std::endl;


	/**
	 * The first argument to the NSE main lua code is the private nse
	 * libaray table which expose certain necessary c functions to the
	 * lua engine.
	 */
	//open_cnse(L);/* first argument, since open_cnse doesn't call lua_pop */

	/**
	 * The second argument is the script rules, including the
	 * files/directories/categories passed as the userdata to this function.
	 */
	lua_createtable(L, rules->size(), 0); /* second argument */
	for(std::vector<std::string>::iterator si = rules->begin(); si != rules->end(); si++)
		nseU_appendfstr(L, -1, "%s", si->c_str());

	lua_call(L, 1, 1);/* returns the NSE main function by nse_main.lua */
	lua_setfield(L, LUA_REGISTRYINDEX, NSE_MAIN);

	return 0;
}


static lua_State *L_NSE = NULL;


void open_nse(void)
{
	if(NULL == L_NSE){

		L_NSE = luaL_newstate();
		if(!L_NSE){
			perror("failed to oopen lua\n");
		}

		lua_settop(L_NSE, 0);

		lua_pushcfunction(L_NSE, init_main);
		lua_pushlightuserdata(L_NSE, &o.scripts);

		if(lua_pcall(L_NSE, 1, 0, 1)){
			fprintf(stderr, "failed to initialize the script engine by running init_main():%s\n",
					lua_tostring(L_NSE,-1));
			lua_settop(L_NSE, 0);
		}
	}
}

static void set_hostinfo(lua_State *L, Target *currenths)
{
	nseU_setsfield(L, -1, "ip", currenths->targetipstr());
}


static int run_main(lua_State *L)
{
	std::vector<Target *> *targets = (std::vector<Target *> *)lua_touserdata(L, 1);

	/* New host group */
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, NSE_CURRENT_HOSTS);

	lua_getfield(L, LUA_REGISTRYINDEX, NSE_MAIN);
	assert(lua_isfunction(L, -1));

	assert(targets->size());

	std::cout << "saiyn: targets: " << targets->front()->targetipstr() << std::endl;

	std::cout << "saiyn: current top: " << lua_gettop(L) << std::endl;

	//lua_state_dump(L);

	/**
	 * The argument to the NSE main function is the list of targets.
	 */
	lua_createtable(L, targets->size(), 0);

	int targets_table = lua_gettop(L);

	lua_getfield(L, LUA_REGISTRYINDEX, NSE_CURRENT_HOSTS);

	int current_hosts = lua_gettop(L);

	for(std::vector<Target *>::iterator it = targets->begin(); it != targets->end(); it++)
	{
		Target *target = (Target *) *it;
		const char *TargetName = target->TargetName();
		const char *targetipstr = target->targetipstr();

		lua_newtable(L);
		set_hostinfo(L, target);
		lua_rawseti(L, targets_table, lua_rawlen(L, targets_table) + 1);

		if(TargetName != NULL && strcmp(TargetName, "") != 0){
			lua_pushstring(L, TargetName);
			lua_pushlightuserdata(L, target);
			lua_rawset(L, current_hosts); /* add to NSE_CURRENT_HOSTS */
		}

		lua_pushstring(L, targetipstr);
		lua_pushlightuserdata(L, target);
		lua_rawset(L, current_hosts);	/* add to NSE_CURRENT_HOSTS */
	}

	std::cout << "saiyn: then top " << lua_gettop(L) << std::endl;

	//lua_state_dump(L);

	lua_settop(L, targets_table);

	lua_call(L, 1, 0);

	return 0;
}

void script_scan(std::vector<Target *> &targets)
{
	
	assert(L_NSE != NULL);
	lua_settop(L_NSE, 0);

	lua_pushcfunction(L_NSE, run_main);
	lua_pushlightuserdata(L_NSE, &targets);
	if(lua_pcall(L_NSE, 1, 0, 1))
		perror("script Engine scan fail\n");

	lua_settop(L_NSE, 0);
}



