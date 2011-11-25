//      Utility.c
//      
//      Copyright 2011 Jonas Kulla <Nyocurio@googlemail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>


struct global_vars
{
	struct stack *digits;
	gint nest_level;
	gint8 frac_point;
	gint8 error_type;
	gboolean debug;
};

gdouble fac(gdouble n)
{
	if (n == 1) {return 1;}
	else if (n/1.0 != (gint)n || n < 1) {return 0;}  // error needed
	else  {return n*fac(n-1);}
}

gdouble modulus(gdouble a, gdouble b)
{
	gdouble quo = a/b;
	return b*(quo-(gint)quo);
}

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
	return ((c >= 42 && c <= 47 && c != 44 && c != 46) || c == 94 || c == 0x21 || c == 0x25);
}

/** determin priority from operator */
gint8 priority(gchar c)
{
		 if (c == '+' || c == '-') {return 0;}
	else if (c == '*' || c == '/') {return 1;}
	else if (c == '^' || c == '%') {return 2;}
	else if (c == '!') {return 3;}
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
		case '%': return modulus(a,b);
		case '!': return fac(a);
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
		g_print("Calc: %f %c %f = %f\n", a, op, b, calc_results);
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
	if (gvars->debug) {g_printf("call: compute_function f_name=%s arg=%f\n", f_name, arg);}

	     if (!g_strcmp0(f_name, "foo"))			{return arg;}
	else if (!g_strcmp0(f_name, "square"))		{return arg*arg;}
	else if (!g_strcmp0(f_name, "sqrt"))		{return sqrt(arg);}
	else if (!g_strcmp0(f_name, "sin"))			{return sin(arg);}
	else if (!g_strcmp0(f_name, "cos"))			{return cos(arg);}
	else if (!g_strcmp0(f_name, "tan"))			{return tan(arg);}
	else if (!g_strcmp0(f_name, "asin"))		{return asin(arg);}
	else if (!g_strcmp0(f_name, "acos"))		{return acos(arg);}
	else if (!g_strcmp0(f_name, "atan"))		{return atan(arg);}

	else {gvars->error_type = 2; return 0;}
}

gdouble lookup_constant(gchar *c_name, struct global_vars *gvars)
{
	if (gvars->debug) {g_printf("call: lookup_constant c_name=%s\n", c_name);}

	     if (!g_strcmp0(c_name, "four"))	{return 4;}
	else if (!g_strcmp0(c_name, "pi"))		{return M_PI;}

	else {gvars->error_type = 2; return 0;}
}

gdouble apply_modifier(gdouble args, gchar mod, gdouble mod_ext)
{
	switch (mod)
	{
		case '*' : return args*mod_ext;
		case '-' : return -1*args;
		case '!' : return args;
		default  : return args;
	}
}


	
