#include <stdlib.h>
#include <stdio.h>


/* ----- Stacks! ------------------------------------------ */

struct node
{
	double content;
	struct node *next;
};

struct stack
{
	int length;
	struct node *top;
};

struct stack* stack_init()
{
	struct stack *lstack = malloc(sizeof(struct stack));
	if (lstack == 0)
		{printf("Cannot initialize stack: out of memory!\n"); return 0;}
	lstack->length = 0; lstack->top = 0;
	return lstack;
}

void stack_push(struct stack *lstack, double contents)
{
	struct node *lnode = malloc(sizeof(struct node));
	if (lnode == 0)
		{printf("Cannot create node: out of memory!\n"); return;}
	lnode->content = contents;
	if (lstack->length == 0) {lnode->next = 0;}
	else {lnode->next = lstack->top;}
	lstack->top = lnode;
	lstack->length += 1;
}

double stack_pop(struct stack *lstack)
{
	if (lstack->length == 0)
		{printf("Warning: stack underflow!\n"); return 0;}
	else
	{
		struct node *popped = lstack->top;
		double contents = popped->content;
		lstack->top = popped->next;
		free(popped);
		lstack->length -= 1;
		return contents;
	}
}

int stack_length(struct stack *lstack)
{
	return lstack->length;
}

void stack_destroy(struct stack *lstack)
{
	while (lstack->length > 0) {stack_pop(lstack);}
	free(lstack);
}

void stack_dump(struct stack *lstack)
{
	struct node *lnode = lstack->top;
	while (lnode != 0)
	{
		printf("%d\n", (int)lnode->content);
		lnode = lnode->next;
	}
}

/* -------------------------------------------------------- */
