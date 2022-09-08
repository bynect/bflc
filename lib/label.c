#include "label.h"
#include <assert.h>

void label_stack_init(Label_Stack *stack, Label_Id *labels, size_t size) {
	assert(stack != NULL);
	assert(labels != NULL && size != 0);

	stack->labels = labels;
	stack->len = 0;
	stack->size = size;
}

void label_stack_push(Label_Stack *stack, Label_Id label) {
	stack->labels[stack->len++] = label;
}

bool label_stack_pop(Label_Stack *stack, Label_Id *label) {
	if (stack->len == 0) return false;
	*label = stack->labels[--stack->len];
	return true;
}

void label_stack_reverse(Label_Stack *stack) {
	for (size_t l = 0, h = stack->len - 1; l < h; l++, h--) {
		Label_Id tmp = stack->labels[l];
		stack->labels[l] = stack->labels[h];
		stack->labels[h] = tmp;
	}
}
