#ifndef LUA_MOD_H
#define LUA_MOD_H

#include "lua.h"

typedef struct
{
    char* name;
    lua_State* state;
}
lua_mod_t;

void mod_load(lua_mod_t* mod, const char *filename);
void mod_call(lua_mod_t* mod, const char *func);

#endif // LUA_MOD_H