#include "include/stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Stack* init_stack(i32 data)
{
	Stack* top = calloc(1, sizeof(Stack));

	top->data = data;
	top->prev = (void*)0;

	return top;
}

bool is_empty(Stack* top)
{
	return top == (void*)0;
}

void push(Stack** top, i32 data, u16* stack_size)
{
	Stack* new_node = init_stack(data);

	new_node->prev = *top;
	*top = new_node;
	(*stack_size)++;
}

i32 pop(Stack** top, u16* stack_size)
{
	if (is_empty(*top)) {
		printf("Stack empty\n");
		exit(1);
		return min_i32;
	}
	i32 data = (*top)->data;
	Stack* popped = *top;
	*top = (*top)->prev;
	free(popped);
	(*stack_size)--;

	return data;
}

void swap(Stack** top)
{
	if (is_empty(*top) || is_empty((*top)->prev)) {
		printf("Stack empty\n");
		exit(1);
	}
	i32 temp = (*top)->prev->data;
	(*top)->prev->data = (*top)->data;
	(*top)->data = temp;
}

i32 peek_top(Stack* top)
{
	if (is_empty(top)) {
		puts("Stack empty");
		exit(1);
		return min_i32;
	}
	return top->data;
}

i32 peek_second(Stack* top)
{
	if (top->prev == (void*)0) {
		puts("No second item");
		exit(1);
	}
	return top->prev->data;
}

void clear(Stack** top, u16* stack_size)
{
	while (!is_empty(*top)) {
		pop(top, stack_size);
	}
}

void sleep_ms(u32 milliseconds) {
    clock_t start = clock();
    clock_t end = start + milliseconds * (CLOCKS_PER_SEC / 1000);
    while (clock() < end) {
        // Do nothing, just loop until time is up
    }
}