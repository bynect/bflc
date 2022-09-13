#ifndef LABEL_H
#define LABEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t Label_Id;

typedef struct {
    Label_Id *labels;
    size_t len;
    size_t size;
} Label_Stack;

void label_stack_init(Label_Stack *stack, Label_Id *labels, size_t size);

void label_stack_push(Label_Stack *stack, Label_Id label);

bool label_stack_pop(Label_Stack *stack, Label_Id *label);

bool label_stack_peek(Label_Stack *stack, Label_Id *label);

void label_stack_reverse(Label_Stack *stack);

#endif
