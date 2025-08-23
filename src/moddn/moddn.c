#include "moddn.h"
#include "files/files.h"

#include <stdio.h>

#ifdef _WIN32
#   include <Windows.h>
#endif // _WIN32

void moddn_entry()
{
    folder_create("moddn");
    folder_create("moddn/config");
    folder_create("moddn/mods");
}

void moddn_exit()
{

}

void moddn_error(const char* message)
{
#ifdef _WIN32
    MessageBoxA(0, "Moddn Error", message, MB_OK | MB_ICONERROR);
    DebugBreak();
#endif // _WIN32

    fprintf(stderr, "MODDN: %s\r\n", message);
}
