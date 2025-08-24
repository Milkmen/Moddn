#include "moddn.h"
#include "files/files.h"
#include "mods/lua_mod.h"

#include <stdio.h>

#define MAX_MODS 64

#ifdef _WIN32
#   include <Windows.h>
#   define SUBHOOK_STATIC 
#else
#   include <dirent.h>
#   include <sys/stat.h>
#endif // _WIN32

#include <subhook.h>

#include <windows.h>
#include <stdio.h>
#include "subhook.h"
#include "moddn.h"
#include "pointers/pointers.h"

#define MAX_MODS 64
#define MAX_PTRS 32

lua_mod_t g_mods[MAX_MODS];
int g_mod_count = 0;

pointer_t g_pointers[MAX_PTRS];
int g_pointer_count = 0;

void load_mods(const char* folder);

void moddn_entry()
{
    folder_create("moddn");
    folder_create("moddn/config");
    folder_create("moddn/mods");

#ifdef _WIN32
    if (!AttachConsole(ATTACH_PARENT_PROCESS))
    {
        AllocConsole();
    }

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    SetConsoleTitleA("Moddn Console");
#endif

    g_pointer_count = ptr_load("moddn/config/pointers.json", g_pointers, MAX_PTRS);
    load_mods("moddn/mods");

    while (1) 
    {
        for (int i = 0; i < g_mod_count; ++i)
        {
            mod_call(&g_mods[i], "on_update");
        }

        Sleep(1000 / 32);
    }
}

void moddn_exit()
{
   

#ifdef _WIN32
    FreeConsole();
#endif
}

void moddn_error(const char* message)
{
    fprintf(stderr, "ERROR: %s\n", message);
#ifdef _WIN32
    MessageBoxA(0, message, "Moddn Error", MB_OK | MB_ICONERROR);
    DebugBreak();
#endif
}

void load_mods(const char* folder)
{
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char search_path[MAX_PATH];

    snprintf(search_path, MAX_PATH, "%s\\*.lua", folder);

    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            char filepath[MAX_PATH];
            snprintf(filepath, MAX_PATH, "%s\\%s", folder, fd.cFileName);
            printf("Loading Mod: %s\n", filepath);
            mod_load(&g_mods[g_mod_count], filepath);
            g_mod_count++;

			if (g_mod_count >= MAX_MODS)
				break;
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

#else
    DIR* dir = opendir(folder);
    if (!dir)
    {
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            size_t len = strlen(entry->d_name);
            if (len > 4 && strcmp(entry->d_name + len - 4, ".lua") == 0)
            {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", folder, entry->d_name);
                printf("Loading Mod: %s\n", filepath);
                mod_load(&g_mods[mod_count], filepath);
                mod_count++;

                if (mod_count >= MAX_MODS)
                    break;
            }
        }
    }

    closedir(dir);
#endif
}