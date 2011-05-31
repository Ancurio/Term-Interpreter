#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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

/** convert char to int */
double to_i(char c)
{
	double j = (double) c;
	return j-48;
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

/** join digits to one int */
double join_digits(struct stack *digits, int frac_point)
{
	int i;
	double ret = 0;
	double temp;
	//int max = digits.length();
	int max = stack_length(digits);
	for (i=frac_point;i<max+frac_point;i++)
	{
		//Double temp = (Double)digits.top(); digits.pop();
		//temp2 = temp.doubleValue();
		temp = stack_pop(digits);
		temp *= pow(10,i);
		ret += temp;
		printf("%f  ",ret);
	}
	return ret;
}

double lookup_constant(char *c_name)
{
	     if (!strcmp(c_name, "four"))	{return 4;}
	else if (!strcmp(c_name, "pi"))		{return M_PI;}
	
	else {return 0;}
	//else {error_type = 2; return 0;}
}









int main(void) {}
