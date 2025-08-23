#include "moddn.h"
#include "files/files.h"
#include "mods/lua_mod.h"

#include <stdio.h>

#define MAX_MODS 64

#ifdef _WIN32
#   include <Windows.h>
#else
#   include <dirent.h>
#   include <sys/stat.h>
#endif // _WIN32

static lua_mod_t mods[MAX_MODS];
static int mod_count = 0;

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
#endif  // _WIN32

	load_mods("moddn/mods");
}

void moddn_exit()
{
#ifdef _WIN32
    FreeConsole();
#endif // _WIN32
}

void moddn_error(const char* message)
{
#ifdef _WIN32
    MessageBoxA(0, "Moddn Error", message, MB_OK | MB_ICONERROR);
    DebugBreak();
#endif // _WIN32

    fprintf(stderr, "ERROR: %s\n", message);
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
            mod_load(&mods[mod_count], filepath);
            mod_count++;

			if (mod_count >= MAX_MODS)
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
                mod_load(&mods[mod_count], filepath);
                mod_count++;

                if (mod_count >= MAX_MODS)
                    break;
            }
        }
    }

    closedir(dir);
#endif
}