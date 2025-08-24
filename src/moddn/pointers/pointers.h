#ifndef POINTERS_H
#define POINTERS_H

#include <stdint.h>

#define NAME_MAX_LEN 32
#define MAX_OFFSETS 8

#define PTR_U8	0
#define PTR_S8	1
#define PTR_U16	2
#define PTR_S16	3
#define PTR_U32	4
#define PTR_S32	5
#define PTR_F32	6
#define PTR_U64	7
#define PTR_S64	8
#define PTR_F64	9

typedef struct
{
	uint8_t type;
	char name[NAME_MAX_LEN];
	char module[NAME_MAX_LEN];
	uintptr_t base;
	uint16_t offsets[MAX_OFFSETS];
	int offset_count;
} 
pointer_t;

int ptr_load(const char* filename, pointer_t* out, int max_pointers);
double ptr_get(const pointer_t* ptr);
void ptr_set(const pointer_t* ptr, double value);

#endif