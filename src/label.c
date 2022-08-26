#include "label.h"
#include <assert.h>

void label_stack_init(Label_Stack *stack, Label_Id *labels, size_t size) {
	stack->labels = labels;
	stack->len = 0;
	stack->size = size;
	stack->fresh = 0;

	assert(labels != NULL && size != 0);
}

void label_stack_push(Label_Stack *stack, Label_Id label) {
	stack->labels[stack->len++] = label;
}

bool label_stack_pop(Label_Stack *stack, Label_Id *label) {
	if (stack->len == 0) return false;
	*label = stack->labels[--stack->len];
	return true;
}

Label_Id label_stack_fresh(Label_Stack *stack) {
	return stack->fresh++;
}
