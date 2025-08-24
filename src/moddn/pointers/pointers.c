#include "pointers.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>

static uint8_t parse_type(const char* type_str)
{
    if (strcmp(type_str, "U8") == 0)  return PTR_U8;
    if (strcmp(type_str, "S8") == 0)  return PTR_S8;
    if (strcmp(type_str, "U16") == 0) return PTR_U16;
    if (strcmp(type_str, "S16") == 0) return PTR_S16;
    if (strcmp(type_str, "U32") == 0) return PTR_U32;
    if (strcmp(type_str, "S32") == 0) return PTR_S32;
    if (strcmp(type_str, "F32") == 0) return PTR_F32;
    if (strcmp(type_str, "U64") == 0) return PTR_U64;
    if (strcmp(type_str, "S64") == 0) return PTR_S64;
    if (strcmp(type_str, "F64") == 0) return PTR_F64;
    return 0xFF; // invalid
}

int ptr_load(const char* filename, pointer_t* out, int max_pointers)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = malloc(len + 1);
    if (!data) { fclose(f); return -2; }

    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    cJSON* root = cJSON_Parse(data);
    free(data);
    if (!root) return -3;

    int count = 0;
    cJSON* child = root->child;

    while (child && count < max_pointers)
    {
        pointer_t* ptr = &out[count];
        memset(ptr, 0, sizeof(pointer_t));

        // Copy the key name
        if (child->string)
        {
            size_t len = strlen(child->string);
            if (len < NAME_MAX_LEN) {
                strcpy(ptr->name, child->string);  // Use strcpy instead of strncpy
            }
            else {
                strncpy(ptr->name, child->string, NAME_MAX_LEN - 1);
                ptr->name[NAME_MAX_LEN - 1] = '\0';
            }
        }

        cJSON* type = cJSON_GetObjectItem(child, "type");
        cJSON* module = cJSON_GetObjectItem(child, "module");
        cJSON* base = cJSON_GetObjectItem(child, "base");
        cJSON* offsets = cJSON_GetObjectItem(child, "offsets");

        if (!type || !module || !base || !offsets) {
            child = child->next;
            continue;
        }

        // Parse type
        if (cJSON_IsString(type) && type->valuestring) {
            ptr->type = parse_type(type->valuestring);
        }

        // Parse module
        if (cJSON_IsString(module) && module->valuestring) {
            size_t len = strlen(module->valuestring);
            if (len < NAME_MAX_LEN) {
                strcpy(ptr->module, module->valuestring);
            }
            else {
                strncpy(ptr->module, module->valuestring, NAME_MAX_LEN - 1);
                ptr->module[NAME_MAX_LEN - 1] = '\0';
            }
        }

        // Parse base - Fix format specifier
        if (cJSON_IsString(base) && base->valuestring) {
            unsigned int base_val;
            if (sscanf(base->valuestring, "%x", &base_val) == 1) {  // Use %x, not %lx
                ptr->base = base_val;
            }
        }

        // Parse offsets
        if (cJSON_IsArray(offsets)) {
            int offset_idx = 0;
            cJSON* off_item = NULL;
            cJSON_ArrayForEach(off_item, offsets) {
                if (offset_idx >= MAX_OFFSETS) break;
                if (cJSON_IsString(off_item) && off_item->valuestring) {
                    unsigned int val;
                    if (sscanf(off_item->valuestring, "%x", &val) == 1) {
                        ptr->offsets[offset_idx++] = (uint16_t)val;
                    }
                }
            }
            ptr->offset_count = offset_idx;
        }

        count++;
        child = child->next;
    }

    cJSON_Delete(root);
    return count;
}

#include <windows.h>

static uintptr_t resolve_pointer(const pointer_t* ptr)
{
    if (!ptr)
        return 0;

    HMODULE hModule = GetModuleHandleA(ptr->module);
    if (!hModule)
        return 0;

    uintptr_t addr = (uintptr_t)hModule + ptr->base;

    for (int i = 0; i < ptr->offset_count; ++i)
    {
        addr = *(uintptr_t*)addr;  // dereference
        addr += ptr->offsets[i];   // add offset
    }

    return addr;
}


double ptr_get(const pointer_t* ptr)
{
    if (!ptr) return 0.0;

    uintptr_t addr = resolve_pointer(ptr);

    switch (ptr->type)
    {
    case PTR_U8:  return (double)*(uint8_t*)addr;
    case PTR_S8:  return (double)*(int8_t*)addr;
    case PTR_U16: return (double)*(uint16_t*)addr;
    case PTR_S16: return (double)*(int16_t*)addr;
    case PTR_U32: return (double)*(uint32_t*)addr;
    case PTR_S32: return (double)*(int32_t*)addr;
    case PTR_F32: return (double)*(float*)addr;
    case PTR_U64: return (double)*(uint64_t*)addr;
    case PTR_S64: return (double)*(int64_t*)addr;
    case PTR_F64: return *(double*)addr;
    default: return 0.0;
    }
}

void ptr_set(const pointer_t* ptr, double value)
{
    if (!ptr) return;

    uintptr_t addr = resolve_pointer(ptr);

    switch (ptr->type)
    {
    case PTR_U8:  *(uint8_t*)addr = (uint8_t)value; break;
    case PTR_S8:  *(int8_t*)addr = (int8_t)value; break;
    case PTR_U16: *(uint16_t*)addr = (uint16_t)value; break;
    case PTR_S16: *(int16_t*)addr = (int16_t)value; break;
    case PTR_U32: *(uint32_t*)addr = (uint32_t)value; break;
    case PTR_S32: *(int32_t*)addr = (int32_t)value; break;
    case PTR_F32: *(float*)addr = (float)value; break;
    case PTR_U64: *(uint64_t*)addr = (uint64_t)value; break;
    case PTR_S64: *(int64_t*)addr = (int64_t)value; break;
    case PTR_F64: *(double*)addr = value; break;
    default:;
    }
}