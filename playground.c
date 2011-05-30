#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

/* -------------------------------------------------------- */

int string_length(char *string)
{
	int i; for (i=0;string[i] != '\0';i++) {}
	return i;
}

/** array wrapper */
char char_at(char *string, int i)
{
	if (i >= 0 && i <= string_length(string)-1)
	{
		return string[i];
	}
	else {return '#';}
}

/** check if char represents an operator */
short isoperator(char c)
{
	int j = (int) c;
	return ((j >= 42 && j <= 47 && j != 44 && j != 46) || j == 94);
}

/** determin priority from operator */
short priority(char c)
{
		 if (c == '+' || c == '-') {return 0;}
	else if (c == '*' || c == '/') {return 1;}
	else if (c == '^') {return 2;}
	else {return 7;}
}

/** carry out primitive arithmetics */
double calculate(double a, double b, char op)
{
	//if (debug) {printf("Calculating!");}
	switch (op)
	{
		case '+': return a+b;
		case '-': return a-b;
		case '*': return a*b;
		case '/': return a/b;
		case '^': return pow(a,b);
		default: return 0;
	}
}

/** compute until operator stack is empty (result is stored in arguments stack) */
void compute(struct stack *arguments, struct stack *operators)
{
	//if (debug) {print("Computing!");}
	//if (arguments.length()-1 != operators.length() && error_type == 0) {error_type = 4;}
	while (stack_length(operators) > 0)
	{
		//Double b_ = (Double)arguments.top(); arguments.pop(); double b = b_.doubleValue();
		double b = stack_pop(arguments);
		//Double a_ = (Double)arguments.top(); arguments.pop(); double a = a_.doubleValue();
		double a = stack_pop(arguments);
		//Character op_ = (Character)operators.top(); operators.pop(); char op = op_.charValue();
		char op = (char) stack_pop(operators);
		//arguments.push(calculate(a, b, op));
		stack_push(arguments, calculate(a, b, op));
	}
}






int main()
{
	//int a, b, c;
	//struct stack *stacky = stack_init();
	//scanf("%d", &a);
	//stack_push(stacky, (double)a);
	//scanf("%d", &a);
	//stack_push(stacky, (double)a);
	//scanf("%d", &a);
	//stack_push(stacky, (double)a);
	//printf(" ---Mirror!---\n");
	//int i; for (i=0;i<3;i++)
	//{ printf("%d\n", (int)stack_pop(stacky)); }
	//printf(" ---Bad stack access!---\n");
	//printf("%d\n", (int)stack_pop(stacky));
	//stack_destroy(stacky);
	//printf(" ---Segfault!---\n");
	//stack_push(stacky, 123);
	char hi[20] = {0};
	scanf("%s",&hi);
	//printf("%d\n", string_length(hi));
	int i;
	//for (i=-2;i<30;i++)
	//{ printf("%c",char_at(hi, i)); }
	struct stack *args = stack_init();
	struct stack *ops = stack_init();
	for (i=0;i<string_length(hi);i++)
	{
		printf("#\n");
		     if (isdigit(hi[i])) {stack_push(args, atoi(&hi[i]));}
		else if (isoperator(hi[i])) {stack_push(ops, (double)hi[i]);}
	}
	//while (stack_length(args) > 0) {printf("%d\n", (int)stack_pop(args));}
	//while (stack_length(ops) > 0) {printf("%c\n", (char)stack_pop(ops));}
	compute(args, ops);
	printf("Result: %d\n", (int)stack_pop(args));
}
