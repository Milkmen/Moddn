#include "moddn.h"
#include "files/files.h"
#include "mods/lua_mod.h"

#include <stdio.h>

#ifdef _WIN32
#   include <Windows.h>
#endif // _WIN32

static FILE* latest_log;

void moddn_entry()
{
    folder_create("moddn");
    folder_create("moddn/config");
    folder_create("moddn/mods");

    latest_log = fopen("moddn/latest.log", "a");

    if(!latest_log)
    {
        moddn_error("Failed to open latest.log");
        return;
    }

    moddn_log(" -- Log Start");

    lua_mod_t test;
    mod_load(&test, "moddn/mods/god.lua");
}

void moddn_exit()
{
    fclose(latest_log);
}

void moddn_error(const char* message)
{
#ifdef _WIN32
    MessageBoxA(0, "Moddn Error", message, MB_OK | MB_ICONERROR);
    DebugBreak();
#endif // _WIN32

    fprintf(stderr, "ERROR: %s\n", message);

    if(!latest_log) return;

    fprintf(latest_log, "ERROR: %s\n", message);
}


void moddn_log(const char* fmt, ...)
{
    if (!latest_log)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(latest_log, fmt, args);
    fprintf(latest_log, "\n");
    fflush(latest_log);
    va_end(args);
}