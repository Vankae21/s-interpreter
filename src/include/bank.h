#ifndef STORAGE_H
#define STORAGE_H

#include "stack.h"
#include "types.h"

typedef struct {
	u16 index;
	i32 data;
} Memory;

typedef struct {
	u16 size;
	Memory** memories;
} Bank;

Bank* init_bank();
void store_memory(Bank* bank, Stack** top, u16* stack_size, u16 index);
void pull_memory(Bank* bank, Stack** top, u16* stack_size, u16 index);
void free_memory(Bank* bank, u16 index);
Memory* get_memory(Bank* bank, u16 index);
bool is_memory_null(Memory* memory);
void echo_bank(Bank* bank);
void free_bank(Bank* bank);
i32 get_memory_data(Bank* bank, u16 index);

#endif