#include "lua_mod.h"

#include "../moddn.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lualib.h>
#include <string.h>
#include "../pointers/pointers.h"

extern pointer_t g_pointers[];
extern int g_pointer_count;

static int lua_pointer_get(lua_State* L);
static int lua_pointer_set(lua_State* L);

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

    lua_register(mod->state, "get", lua_pointer_get);
    lua_register(mod->state, "set", lua_pointer_set);

    mod_call(mod, "on_load");
}

void mod_call(lua_mod_t* mod, const char *func)
{
    if (!mod->state)
        return;

    lua_getglobal(mod->state, func);
    if (!lua_isfunction(mod->state, -1)) 
    {
        lua_pop(mod->state, 1);
        //moddn_error("Function not found in Lua mod");
        return;
    }

    if (lua_pcall(mod->state, 0, 0, 0) != 0) 
    {
        moddn_error(lua_tostring(mod->state, -1));
        lua_pop(mod->state, 1);
    }
}

static int lua_pointer_get(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    double value = ptr_get(name);
    lua_pushnumber(L, value);
    return 1;
}

static int lua_pointer_set(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    double value = luaL_checknumber(L, 2);
    ptr_set(name, value);
    return 0;
}

