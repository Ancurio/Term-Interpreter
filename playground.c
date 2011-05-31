#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

const static int TRUE = 1;
const static int FALSE = 0;
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

struct global_vars
{
	struct stack *digits;
	int nest_level;
	int error_type;
};

int string_length(char *string)
{
	int i; for (i=0;string[i] != '\0';i++) {}
	return i;
}

void string_concat(char *string, char c)
{
	int l = string_length(string);
	string[l] = c; string [l+1] = 0;
}

void string_clear(char *string)
{
	int i, l;
	l = string_length(string);
	for (i=0;i<l;i++) {string[i] = 0;}
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

/** convert char to int */
double to_i(char c)
{
	double j = (double) c;
	return j-48;
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


double compute_function(char *f_name, double arg)
{
	//if (debug) {print(f_name);}
	
		 //if (f_name.equals("foo"))        {return arg;}
	     if (!strcmp(f_name, "foo"))		{return arg;}
	//else if (f_name.equals("square"))     {return arg*arg;}
	else if (!strcmp(f_name, "square"))		{return arg*arg;}
	
	//else {error_type = 2; return 0;}
	else {return 0;}
}

double lookup_constant(char *c_name)
{
	     if (!strcmp(c_name, "four"))	{return 4;}
	else if (!strcmp(c_name, "pi"))		{return M_PI;}
	
	else {return 0;}
	//else {error_type = 2; return 0;}
}

double _parse(char *args, int *error_type)
{
	//Stack arguments = new Stack();  /** holds float values */
	//Stack operators = new Stack();  /** holds arithmetic operators */
	struct stack *arguments = stack_init();
	struct stack *operators = stack_init();
	struct stack *digits = stack_init();
    
    int nest_level = 0;
    
	int i = 0;
	int local_nest_level = 0;
	short last_p = 0;  /** priority of last parsed operator */
	short frac_point = 0;
	short coef_flag = FALSE;  /** set if value might preceed a bracket and thus become a coefficient */
	short func_flag = FALSE;  /** set if result of next bracket is to be passed as an argument to function <symbol> */
	short nest_flag = FALSE;  /** indicates characters are being collected and not parsed */
	short nest_init_flag = FALSE;  /** necessary to skip first character '(' during string collection */
	short neg_flag = TRUE;  /** set if next '-' will make number signed and not be an operator */
	char nested_term[100] = { 0 }; /** collector string for contents of nested term */
	char symbol[100] = { 0 }; /** collector string for symbol name */
	short frac_flag = FALSE;
	
	
	//char[] args = arg.toCharArray(); /** map out String to pure char array */
	for (i=0; i < string_length(args); i++)
	{
		
		if (nest_init_flag) {nest_init_flag = FALSE;}
		
		/** lock computing by raising nest level, substitute '*' if coefficient exists */
		if (args[i] == '(')
		{
			if (!nest_flag) /** nested interpreting is just about to be initialized */
			{
				//if (coef_flag) {operators.push('*'); last_p = priority('*');}
				coef_flag = TRUE;
				nest_flag = TRUE;
				nest_init_flag = TRUE;
				nest_level += 1;
			}
			else  /** nested interpreting is in progress */
			{
				local_nest_level += 1;
			}
		}
		
		else if (args[i] == ')')
		{
			if (nest_flag && local_nest_level == 0) /** nesting has reached end */
			{
				nest_flag = FALSE; nest_level -= 1;
				double nested_term_result = _parse(nested_term, error_type);
				//nested_term = "";
				if (func_flag)
				{
					//arguments.push(compute_function(nested_term_result, symbol));
					func_flag = FALSE;
					//symbol = "";
				}
				//else {arguments.push(nested_term_result);}
			}
			else  /** nested interpreting is in progress */
			{
				local_nest_level -= 1;
			}
		}
		
		
		if (!nest_flag)
		{
			
			if (args[i] == '.' || args[i] == ',')
			{
				if (isdigit(char_at(args,i+1))) {frac_flag = TRUE;}
				else {*error_type = 3; return 0;}
			}
		
			else if (isdigit(args[i]))  /** parse number */
			{
				if (frac_flag) {frac_point -= 1;}
				/** check if there is more than one digit or fractal part */
				if (isdigit(char_at(args, i+1)) || char_at(args, i+1) == '.' || char_at(args, i+1) == ',') 
					{}//{digits.push(to_i(args[i]));}
				else
				{
					//digits.push(to_i(args[i]));
					//if (coef_flag) {operators.push('*'); last_p = priority('*');}
					//arguments.push(join_digits(frac_point));
					neg_flag = FALSE; coef_flag = TRUE; frac_flag = FALSE; frac_point = 0;
				}
			}
		
			else if (isoperator(args[i]))  /** parse operators */
			{
				if (neg_flag && args[i] == '-')  /** check if preceeding minus changes sign of next symbol */
				{
					neg_flag = FALSE;
					//arguments.push(-1.0); operators.push('*'); last_p = priority('*');
				}
				else
				{
					//if (arguments.length() <= operators.length()) {error_type = 4; break;}
					/** check beforehand if lower priority operator is encountered */
					if (priority(args[i]) < last_p) {if (nest_level == 0) {compute(arguments, operators);}}
					last_p = priority(args[i]);
					//operators.push(args[i]);
					coef_flag = FALSE;
					neg_flag = TRUE;
				}
			}
			
			else if (isalpha(args[i])) /** parse letters */
			{
				//if (coef_flag) {coef_flag = FALSE; operators.push('*'); last_p = priority('*');}
				//symbol = symbol.concat(to_s(args[i]));
				if (char_at(args,i+1) == '(')
				{
					compute_function(symbol, 0);
					if (*error_type != 0)
					{
						*error_type = 0;
						//arguments.push(lookup_constant(symbol));
						//symbol = "";
						coef_flag = TRUE;
					}
					else {func_flag = TRUE;}
				}
				else if (!isalpha(char_at(args,i+1)))
				{
					//arguments.push(lookup_constant(symbol));
					//symbol = "";
					coef_flag = TRUE;
				}
			}
		
		}
		
		else if (!nest_init_flag) /** this collector block needs to be skipped once so the first '(' isn't collected */
		{
			//Character ch = args[i];
			//nested_term = nested_term.concat(ch.toString());
		}
		
		if (args[i] == ' ') {coef_flag = FALSE;}
		
		if (char_at(args,i) == '#') {break;}  /** failsafe, in case array bounds are left */
	}
	// dump_stacks();
	if ((nest_level != 0 || local_nest_level != 0) && *error_type == 0) {*error_type = 1;}
	if (neg_flag && *error_type == 0) {*error_type = 4;}
	if (*error_type == 0) {compute(arguments, operators);}
	//if (arguments.length() > 1 && error_type == 0) {error_type = 4;}
	//if (error_type == 0) {return (Double)arguments.top();}
	else {return 0;}
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
	//struct stack *args = stack_init();
	//struct stack *ops = stack_init();
	//struct stack *digs = stack_init();
	//for (i=0;i<string_length(hi);i++)
	//{
		//printf("#\n");
		     //if (isdigit(hi[i])) {stack_push(digs, to_i(hi[i]));}
		//else if (isoperator(hi[i])) {stack_push(ops, (double)hi[i]); printf("%d",priority(hi[i]));}
	//}
	//while (stack_length(args) > 0) {printf("%d\n", (int)stack_pop(args));}
	//while (stack_length(ops) > 0) {printf("%c\n", (char)stack_pop(ops));}
	//compute(args, ops);
	//printf("Result: %d\n", (int)stack_pop(args));
	//printf("<Args>\n");
	//stack_dump(args);
	//printf("<Ops>\n");
	//stack_dump(ops);
	//printf("%g\n",join_digits(digs, 0));
	//printf("%f\n", compute_function("foo", 234));
	//printf("%f\n", compute_function("a", 13));
	//printf("%f\n", compute_function("square", 4));
	//printf("%f\n", lookup_constant("pi"));
	printf("%s\n", hi);
	string_concat(hi, 'x');
	printf("%s\n", hi);
	string_concat(hi, 'y');
	printf("%s\n", hi);
	string_clear(hi);
	printf("%s\n", hi);
	return 0;
}
