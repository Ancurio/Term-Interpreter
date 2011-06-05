#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>


/* ----- Stacks! ------------------------------------------ */

struct node
{
	void *content;
	struct node *next;
};

struct stack
{
	int length;
	short content_size;
	struct node *top;
};

struct stack* stack_init(short content_size)
{
	struct stack *lstack = malloc(sizeof(struct stack));
	if (lstack == 0)
		{printf("Cannot initialize stack: out of memory!\n"); return 0;}
	lstack->length = 0; lstack->top = 0;
	lstack->content_size = content_size;
	return lstack;
}

void stack_push(struct stack *lstack, void *push_content)
{
	struct node *lnode = malloc(sizeof(struct node));
	void *lcontent = malloc( lstack->content_size );
	if (lnode == 0 || lcontent == 0)
		{printf("Cannot create node: out of memory!\n"); return;}
		
	memcpy(lcontent, push_content, lstack->content_size);
	lnode->content = lcontent;
	if (lstack->length == 0) {lnode->next = 0;}
	else {lnode->next = lstack->top;}
	lstack->top = lnode;
	lstack->length += 1;
}

void stack_pop(struct stack *lstack, void *return_location)
{
	if (lstack->length == 0)
		{printf("Warning: stack underflow!\n");}
	else
	{
		struct node *popped = lstack->top;
		if (return_location != 0)
			{memcpy(return_location, popped->content, lstack->content_size);}
		lstack->top = popped->next;
		free(popped->content);
		free(popped);
		lstack->length -= 1;
	}
}

int stack_length(struct stack *lstack)
{
	return lstack->length;
}

void stack_destroy(struct stack *lstack)
{
	while (lstack->length > 0) {stack_pop(lstack, 0);}
	free(lstack);
}

void stack_dump(struct stack *lstack)
{
	struct node *lnode = lstack->top;
	while (lnode != 0)
	{
		printf("%d\n", (int*)lnode->content);
		lnode = lnode->next;
	}
}

/* -------------------------------------------------------- */

main(void)
{
	struct stack *mystack = stack_init(sizeof(char));
	struct stack *float_stack = stack_init(sizeof(float));
	char c = 'a';
	float f = 3.2;
	stack_push(mystack, &c);
	stack_push(float_stack, &f);
	c = 'b'; stack_push(mystack, &c);
	f = 1.3; stack_push(float_stack, &f);
	c = 'c'; stack_push(mystack, &c);
	f = 4.8134; stack_push(float_stack, &f);
	f = 1.337; stack_push(float_stack, &f);
	char c2 = 0;
	float f2 = 0.0;
	int i;
	for (i=0;i<4;i++)
	{
		stack_pop(mystack, &c2);
		printf("%c\n", c2);
		stack_pop(float_stack, &f2);
		printf("%f\n", f2);
	}
	stack_destroy(mystack);
	stack_destroy(float_stack);
}
