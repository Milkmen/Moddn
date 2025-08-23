#include "lua_mod.h"

#include "../moddn.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int lua_func_print(lua_State* L);

void mod_load(lua_mod_t* mod, const char *filename)
{
    mod->state = luaL_newstate();
    if(!mod->state)
    {
        moddn_error("Failed to create Lua state");
        return;
    }

    luaL_openlibs(mod->state);

    if(luaL_loadfile(mod->state, filename) || lua_pcall(mod->state, 0, 0, 0))
    {
        moddn_error("Failed to load Lua file");
        lua_close(mod->state);
        mod->state = 0;
        return;
    }

    lua_pushcfunction(mod->state, lua_func_print);
    lua_setglobal(mod->state, "print");

    mod_call(mod, "on_load");
}

void mod_call(lua_mod_t* mod, const char *func)
{
    if (!mod->state)
        return;

    lua_getglobal(mod->state, func);
    if (!lua_isfunction(mod->state, -1)) {
        lua_pop(mod->state, 1);
        moddn_error("Function not found in Lua mod");
        return;
    }

    if (lua_pcall(mod->state, 0, 0, 0) != 0) {
        moddn_error(lua_tostring(mod->state, -1));
        lua_pop(mod->state, 1);
    }
}

int lua_func_print(lua_State* L)
{
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) 
    {
        moddn_log("%s", lua_tostring(L, i));
    }
    return 0;
}