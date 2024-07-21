#ifndef STACK_H
#define STACK_H

#include "types.h"

typedef struct Stack {
	i32 data;
	struct Stack* prev;
} Stack;

Stack* init_stack(i32 data);
bool is_empty(Stack* top);
void push(Stack** top, i32 data, u16* stack_size);
i32 pop(Stack** top, u16* stack_size);
void swap(Stack** top);
i32 peek_top(Stack* top);
i32 peek_second(Stack* top);
void clear(Stack** top, u16* stack_size);
void sleep_ms(u32 milliseconds);

#endif