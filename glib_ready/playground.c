#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <glib.h>
#include <glib/gprintf.h>

/* ----- Stack.c ---------------------------------------------------- */
struct node
{
	gpointer content;
	struct node *next;
};

struct stack
{
	gint length;
	gint content_size;
	struct node *top;
};

struct stack* stack_init(gint content_size)
{
	struct stack *lstack = g_try_malloc(sizeof(struct stack));
	if (lstack == 0)
		{g_error("Cannot initialize stack: out of memory!\n"); return 0;}
	lstack->length = 0; lstack->top = 0;
	lstack->content_size = content_size;
	return lstack;
}

void stack_push(struct stack *lstack, gpointer push_content)
{
	struct node *lnode = g_try_malloc(sizeof(struct node));
	void *lcontent = g_try_malloc( lstack->content_size );
	if (lnode == 0 || lcontent == 0)
		{g_error("Cannot create node: out of memory!\n"); return;}
		
	memcpy(lcontent, push_content, lstack->content_size);
	lnode->content = lcontent;
	if (lstack->length == 0) {lnode->next = 0;}
	else {lnode->next = lstack->top;}
	lstack->top = lnode;
	lstack->length += 1;
}

void stack_pop(struct stack *lstack, gpointer return_location)
{
	if (lstack->length == 0)
		{g_warning("stack underflow!\n");}
	else
	{
		struct node *popped = lstack->top;
		if (return_location != 0)
			{memcpy(return_location, popped->content, lstack->content_size);}
		lstack->top = popped->next;
		g_free(popped->content);
		g_free(popped);
		lstack->length -= 1;
	}
}

gint stack_length(struct stack *lstack)
{
	return lstack->length;
}

void stack_destroy(struct stack *lstack)
{
	while (lstack->length > 0) {stack_pop(lstack, 0);}
	g_free(lstack);
}

void stack_dump(struct stack *lstack, gchar mode)
{
	struct node *lnode = lstack->top;
	while (lnode != 0)
	{
		gpointer lpointer = lnode->content;
		     if (mode == 'd')
		{
			gdouble lcontent = *(gdouble *)lpointer;
			g_printf("%f\n", lcontent);
		}
		else if (mode == 'c')
		{
			gchar lcontent = *(gchar *)lpointer;
			g_printf("%c\n", lcontent);
		}
		lnode = lnode->next;
	}
}

/* ----- Stack.c END ------------------------------------------------ */

/* ----- Utility.c -------------------------------------------------- */

struct global_vars
{
	struct stack *digits;
	gint nest_level;
	gint8 frac_point;
	gint8 error_type;
	gboolean debug;
};

/** array wrapper */
gchar char_at(gchar *string, gint i)
{
	if (i >= 0 && i <= strlen(string)-1)
	{
		return string[i];
	}
	else {return '#';}
}

/** check if char represents an operator */
gboolean isoperator(gchar c)
{
	return ((c >= 42 && c <= 47 && c != 44 && c != 46) || c == 94);
}

/** determin priority from operator */
gint8 priority(gchar c)
{
		 if (c == '+' || c == '-') {return 0;}
	else if (c == '*' || c == '/') {return 1;}
	else if (c == '^') {return 2;}
	else {return 7;}
}

/** carry out primitive arithmetics */
gdouble calculate(gdouble a, gdouble b, gchar op)
{
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
void compute(struct stack *arguments, struct stack *operators, struct global_vars *gvars)
{
	if (stack_length(arguments)-1 != stack_length(operators) && gvars->error_type == 0)
		{gvars->error_type = 4;}
	while (stack_length(operators) > 0)
	{
		gdouble b; stack_pop(arguments, &b);
		gdouble a; stack_pop(arguments, &a);
		gchar op; stack_pop(operators, &op);
		gdouble calc_results = calculate(a, b, op);
		stack_push(arguments, &calc_results);
	}
}

/** convert char to double */
gdouble to_d(char c)
{
	return c-48;
}

/** join digits to one int */
gdouble join_digits(struct stack *digits, gint8 frac_point)
{
	gint8 i;
	gdouble ret = 0;
	gint8 temp_int8;
	gdouble temp;
	gint max = stack_length(digits);
	for (i=frac_point;i<max+frac_point;i++)
	{
		stack_pop(digits, &temp_int8);
		temp = temp_int8;
		temp *= pow(10,i);
		ret += temp;
		//printf("joining...%f  \n", ret);
	}
	return ret;
}


gdouble compute_function(gchar *f_name, gdouble arg, struct global_vars *gvars)
{
	if (gvars->debug) {printf("call: compute_function f_name=%s arg=%f\n", f_name, arg);}

	     if (!g_strcmp0(f_name, "foo"))		{return arg;}
	else if (!g_strcmp0(f_name, "square"))		{return arg*arg;}

	else {gvars->error_type = 2; return 0;}
}

gdouble lookup_constant(gchar *c_name, struct global_vars *gvars)
{
	if (gvars->debug) {printf("call: lookup_constant c_name=%s\n", c_name);}

	     if (!g_strcmp0(c_name, "four"))	{return 4;}
	else if (!g_strcmp0(c_name, "pi"))		{return M_PI;}

	else {gvars->error_type = 2; return 0;}
}

/* ----- Utility.c END ---------------------------------------------- */

/* ----- Parse.c ---------------------------------------------------- */

double _parse(gchar *args, struct global_vars *gvars)
{
	gchar mul_char = '*';
	gdouble minus_one = -1.0;
	gchar null = 0;
	gint args_len = strlen(args);
	
	struct stack *arguments = stack_init(sizeof(gdouble));
	struct stack *operators = stack_init(sizeof(gchar));

	gint i = 0;
	gint j = 0;
	gint local_nest_level = 0;

	gint8 last_p = 0;  /** priority of last parsed operator */
	gboolean coef_flag = FALSE;  /** set if value might preceed a bracket and thus become a coefficient */
	gboolean func_flag = FALSE;  /** set if result of next bracket is to be passed as an argument to function <symbol> */
	gboolean nest_flag = FALSE;  /** indicates characters are being collected and not parsed */
	gboolean nest_init_flag = FALSE;  /** necessary to skip first character '(' during string collection */
	gboolean neg_flag = TRUE;  /** set if next '-' will make number signed and not be an operator */
	gboolean frac_flag = FALSE;
	
	
	//char nested_term[100] = { 0 }; /** collector string for contents of nested term */
	GString *nested_term = g_string_new(&null);
	//char symbol[100] = { 0 }; /** collector string for symbol name */
	GString *symbol = g_string_new(&null);


	for (i=0; i < args_len; i++)
	{

		if (nest_init_flag) {nest_init_flag = FALSE;}

		/** lock computing by raising nest level, substitute '*' if coefficient exists */
		if (args[i] == '(')
		{
			if (!nest_flag) /** nested interpreting is just about to be initialized */
			{
				if (coef_flag) {stack_push(operators, &mul_char); last_p = priority(mul_char);}
				coef_flag = TRUE;
				nest_flag = TRUE;
				nest_init_flag = TRUE;
				gvars->nest_level += 1;
				nested_term = g_string_new(&null);
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
				nest_flag = FALSE;
				gdouble nested_term_result = _parse(nested_term->str, gvars);
				gvars->nest_level -= 1;
				g_string_free(nested_term, TRUE);
				nested_term = g_string_new(&null);
				if (func_flag)
				{
					gdouble compute_function_results = compute_function(symbol->str, nested_term_result, gvars);
					stack_push(arguments, &compute_function_results);
					func_flag = FALSE;
					g_string_free(symbol, TRUE);
					symbol = g_string_new(&null);
				}
				else {stack_push(arguments, &nested_term_result);}
			}
			else  /** nested interpreting is in progress, passing by uninterpreted ')' */
			{
				local_nest_level -= 1;
			}
		}


		if (!nest_flag)
		{

			if (args[i] == '.' || args[i] == ',')
			{
				if (g_ascii_isdigit(char_at(args,i+1))) {frac_flag = TRUE;}
				else {gvars->error_type = 3; return 0;}
			}

			else if (g_ascii_isdigit(args[i]))  /** parse number */
			{
				if (gvars->debug)
					{for (j=0;j<gvars->nest_level;j++) {g_printf("   ");} g_printf("args[%d] is digit\n", i);}

				gint8 dig = to_d(args[i]);
				stack_push(gvars->digits, &dig);
				if (frac_flag) {gvars->frac_point -= 1;}
				/** check if there is more than one digit or fractal part */
				if (!(g_ascii_isdigit(char_at(args, i+1)) || char_at(args, i+1) == '.' || char_at(args, i+1) == ','))
				{
					if (coef_flag) {stack_push(operators, &mul_char); last_p = priority(mul_char);}
					gdouble joined_dig =  join_digits(gvars->digits, gvars->frac_point);
					stack_push(arguments, &joined_dig);
					neg_flag = FALSE; coef_flag = TRUE; frac_flag = FALSE; gvars->frac_point = 0;
				}
			}

			else if (isoperator(args[i]))  /** parse operators */
			{
				if (gvars->debug)
					{for (j=0;j<gvars->nest_level;j++) {g_printf("   ");} g_printf("args[%d] is operator\n", i);}

				if (neg_flag && args[i] == '-')  /** check if preceeding minus changes sign of next symbol */
				{
					neg_flag = FALSE;
					stack_push(arguments, &minus_one); stack_push(operators, &mul_char); last_p = priority(mul_char);
				}
				else
				{
					if (stack_length(arguments) <= stack_length(operators)) {gvars->error_type = 4; break;}
					/** check beforehand if lower priority operator is encountered */
					if (priority(args[i]) < last_p) {compute(arguments, operators, gvars);}
					last_p = priority(args[i]);
					stack_push(operators, &args[i]);
					coef_flag = FALSE;
					neg_flag = TRUE;
				}
			}

			else if (g_ascii_isalpha(args[i])) /** parse letters */
			{
				if (gvars->debug)
					{for (j=0;j<gvars->nest_level;j++) {g_printf("   ");} printf("args[%d] is letter\n", i);}

				if (coef_flag) {coef_flag = FALSE; stack_push(operators, &mul_char); last_p = priority(mul_char);}
				if (neg_flag) {neg_flag = FALSE;}
				g_string_append_c(symbol, args[i]);
				if (char_at(args,i+1) == '(')
				{
					compute_function(symbol->str, 1.337, gvars);
					if (gvars->error_type != 0)
					{
						gvars->error_type = 0;
						gdouble looked_up_c = lookup_constant(symbol->str, gvars);
						stack_push(arguments, &looked_up_c);
						//+symbol = "";
						g_string_free(symbol, TRUE);
						symbol = g_string_new(&null);
						coef_flag = TRUE;
					}
					else {func_flag = TRUE;}
				}
				else if (!g_ascii_isalpha(char_at(args,i+1)))
				{
					gdouble looked_up_c = lookup_constant(symbol->str, gvars);
					stack_push(arguments, &looked_up_c);
					g_string_free(symbol, TRUE);
					symbol = g_string_new(&null);
					coef_flag = TRUE;
				}
			}

		}

		else if (!nest_init_flag) /** this collector block needs to be skipped once so the first '(' isn't collected */
		{
			g_string_append_c(nested_term, args[i]);
		}

		if (args[i] == ' ') {coef_flag = FALSE;}

		if (char_at(args,i) == '#') {break;}  /** failsafe, in case array bounds are left */
	}
	if (gvars->debug)
		{printf("<args>\n");stack_dump(arguments, 'd');printf("<ops>\n");stack_dump(operators, 'c');printf("<>\n");}

	if (local_nest_level != 0 && gvars->error_type == 0) {gvars->error_type = 1;}
	if (neg_flag && gvars->error_type == 0) {gvars->error_type = 4;}
	if (gvars->error_type == 0) {compute(arguments, operators, gvars);}
	if (stack_length(arguments) > 1 && gvars->error_type == 0) {gvars->error_type = 4;printf("no2\n");}

	gdouble return_value = 0;
	if (gvars->error_type == 0) {stack_pop(arguments, &return_value);}
	stack_destroy(arguments);
	stack_destroy(operators);

	return return_value;
}

gdouble parse(gchar *args, gint8 *_error_type, gboolean _debug)
{
	struct global_vars *gvars = malloc(sizeof(struct global_vars));
	gvars->digits = stack_init(sizeof(gint8));
	gvars->nest_level = 0;
	gvars->frac_point = 0;
	gvars->error_type = 0;
	gvars->debug = _debug;

	gdouble results = _parse(args, gvars);
	if (gvars->nest_level != 0 && gvars->error_type == 0)
		{gvars->error_type = 1;}
	*_error_type = gvars->error_type;
	stack_destroy(gvars->digits);
	free(gvars);
	return results;
}

/* ----- Parse.c END ------------------------------------------------ */


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
	//int i;
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
	//printf("%s\n", hi);
	//string_concat(hi, 'x');
	//printf("%s\n", hi);
	//string_concat(hi, 'y');
	//printf("%s\n", hi);
	//string_clear(hi);
	//printf("%s\n", hi);
	gint8 t;
	printf("Results!!: %f\n", parse(hi, &t, TRUE));
	printf("Errors... : %d\n", t);
	return 0;
}
