#include "files.h"

#include "../moddn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h> // for _mkdir
#define PATH_SEPARATOR '\\'
#define mkdir_os(path) _mkdir(path)
#else
#include <unistd.h>
#define PATH_SEPARATOR '/'
#define mkdir_os(path) mkdir(path, 0755)
#endif

bool folder_exists(const char* path)
{
	struct stat st;
	return (stat(path, &st) == 0 && (st.st_mode & S_IFDIR));
}

bool folder_create(const char* path)
{
    if (!folder_exists(path)) 
    {
        if (mkdir_os(path) != 0) 
        {
            moddn_error("Failed to create folder");
            return 0;
        }
    }
    return 1;
}