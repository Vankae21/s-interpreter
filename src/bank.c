#include "include/bank.h"
#include <stdio.h>
#include <stdlib.h>

Bank* init_bank()
{
	Bank* bank = calloc(1, sizeof(bank));

	bank->size = 0;
	bank->memories = (void*)0;

	return bank;
}

void store_memory(Bank* bank, Stack** top, u16* stack_size, u16 index)
{
	Memory* memory = get_memory(bank, index);
	if (!is_memory_null(memory)) {
		printf("Memory address is full\n");
		exit(1);
	}
	bank->size++;
	bank->memories = realloc(bank->memories, bank->size * sizeof(Memory*));
	memory = calloc(1, sizeof(Memory));
	memory->index = index;
	memory->data = pop(top, stack_size);
	bank->memories[bank->size - 1] = memory;
}

void pull_memory(Bank* bank, Stack** top, u16* stack_size, u16 index)
{
	Memory* memory = get_memory(bank, index);
	if (is_memory_null(memory)) {
		printf("Memory address is null\n");
		exit(1);
	}
	push(top, memory->data, stack_size);
	free_memory(bank, index);
}

void free_memory(Bank* bank, u16 index)
{
	Memory* memory = get_memory(bank, index);
	if (is_memory_null(memory)) {
		printf("No Data at Address: %d\n", index);
		exit(1);
	}

	u16 c = 0;
	Memory** result = (void*)0;
	for (int i = 0; i < bank->size; i++) {
		if (bank->memories[i]->index == index) {
			free(bank->memories[i]);
			bank->memories[i] = (void*)0;
			continue;
		}
		c++;
		result = realloc(result, c * sizeof(Memory*));
		result[c - 1] = bank->memories[i];
	}
	free(bank->memories);
	bank->size = c;
	bank->memories = result;
}

Memory* get_memory(Bank* bank, u16 index)
{
	Memory* memory = (void*)0;
	for (int i = 0; i < bank->size; i++) {
		if (bank->memories[i]->index == index) {
			memory = bank->memories[i];
			break;
		}
	}
	return memory;
}

bool is_memory_null(Memory* memory)
{
	return memory == (void*)0;
}

void echo_bank(Bank* bank)
{
	for (int i = 0; i < bank->size; i++) {
		printf("%d ; at Address: %d\n", bank->memories[i]->data, bank->memories[i]->index);
	}
}

void free_bank(Bank* bank)
{
	for (int i = 0; i < bank->size; i++) {
		free(bank->memories[i]);
	}
	free(bank->memories);
}

i32 get_memory_data(Bank* bank, u16 index)
{
	Memory* memory = get_memory(bank, index);
	if (is_memory_null(memory)) {
		printf("No Data at Address: %d\n", index);
		exit(1);
	}
	return memory->data;
}