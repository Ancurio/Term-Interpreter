//      Parse.c
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
#include <string.h>
#include <math.h>


#include "Stack.h"
#include "Utility.h"


void flag_changed(gboolean flag, char *s)
{
	g_print("Flag set to %d in %s\n", flag, s);
}


double _parse(gchar *args, struct global_vars *gvars)
{
	gdouble minus_one = -1.0;
	gdouble null = 0;
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
	gboolean value_flag = FALSE; /** indicates previously parsed value as opposed to an operator */
	gboolean frac_flag = FALSE;
	
	gboolean no_input_flag = FALSE;
	if (strlen(args) == 0) {no_input_flag = TRUE;}
	
	GString *nested_term = g_string_new("\0"); /** collector string for contents of nested term */
	GString *symbol = g_string_new("\0"); /** collector string for symbol name */

	if (!no_input_flag) {
		for (i=0; i < args_len; i++)
		{
	
			if (nest_init_flag) {nest_init_flag = FALSE;}
	
			/** lock computing by raising nest level, substitute '*' if coefficient exists */
			if (args[i] == '(')
			{
				if (!nest_flag) /** nested interpreting is just about to be initialized */
				{
					if (coef_flag) {stack_push(operators, "*"); last_p = priority('*');}
					coef_flag = TRUE;
					nest_flag = TRUE;
					nest_init_flag = TRUE;
					gvars->nest_level += 1;
					nested_term = g_string_new("\0");
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
					nest_flag = FALSE; value_flag = TRUE;
					gdouble nested_term_result = _parse(nested_term->str, gvars);
					gvars->nest_level -= 1;
					g_string_free(nested_term, TRUE);
					nested_term = g_string_new("\0");
					if (func_flag)
					{
						gdouble compute_function_results =
							compute_function(symbol->str, nested_term_result, gvars);
						stack_push(arguments, &compute_function_results);
						func_flag = FALSE;
						g_string_free(symbol, TRUE);
						symbol = g_string_new("\0");
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
						if (coef_flag) {stack_push(operators, "*"); last_p = priority('*');}
						gdouble joined_dig =  join_digits(gvars->digits, gvars->frac_point);
						stack_push(arguments, &joined_dig);
						coef_flag = TRUE; frac_flag = FALSE; value_flag = TRUE; gvars->frac_point = 0;
					}
				}
	
				else if (isoperator(args[i]))  /** parse operators */
				{
					if (gvars->debug)
						{for (j=0;j<gvars->nest_level;j++) {g_printf("   ");} g_printf("args[%d] is operator\n", i);}
	
					if (args[i] == '-')  /** check if preceeding minus changes sign of next symbol */
					{
						if (value_flag)
						{
							compute(arguments, operators, gvars);
							stack_push(operators, "+");
						}
						stack_push(arguments, &minus_one);
						stack_push(operators, "*"); last_p = priority('*');
						
						coef_flag = FALSE;
					}
					else
					{
						if (stack_length(arguments) <= stack_length(operators)) {gvars->error_type = 4; break;}
						/** check beforehand if lower priority operator is encountered */
						if (priority(args[i]) < last_p) {compute(arguments, operators, gvars);}
						last_p = priority(args[i]);
						stack_push(operators, &args[i]);
						if (args[i] == '!') {stack_push(arguments, &null);}  // dummy zero to avoid complications due to missing second args for faculty
						coef_flag = FALSE;
						value_flag = FALSE;
					}
				}
	
				else if (g_ascii_isalpha(args[i])) /** parse letters */
				{
					if (gvars->debug)
						{for (j=0;j<gvars->nest_level;j++) {g_printf("   ");} printf("args[%d] is letter\n", i);}
	
					if (coef_flag) {coef_flag = FALSE; stack_push(operators, "*"); last_p = priority('*');}
					g_string_append_c(symbol, args[i]);
					if (char_at(args,i+1) == '(')
					{
						compute_function(symbol->str, 0, gvars);
						if (gvars->error_type != 0)
						{
							gvars->error_type = 0;
							gdouble looked_up_c = lookup_constant(symbol->str, gvars);
							stack_push(arguments, &looked_up_c);
							g_string_free(symbol, TRUE);
							symbol = g_string_new("\0");
							coef_flag = TRUE;
							value_flag = TRUE;
						}
						else {func_flag = TRUE;}
					}
					else if (!g_ascii_isalpha(char_at(args,i+1)))
					{
						gdouble looked_up_c = lookup_constant(symbol->str, gvars);
						stack_push(arguments, &looked_up_c);
						g_string_free(symbol, TRUE);
						symbol = g_string_new("\0");
						coef_flag = TRUE;
						value_flag = TRUE;
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
	}
	if (gvars->debug)
		{printf("<args>\n");stack_dump(arguments, 'd');printf("<ops>\n");stack_dump(operators, 'c');printf("<>\n");
		printf("errors so far: %d\n", gvars->error_type);}

	if (local_nest_level != 0 && gvars->error_type == 0) {gvars->error_type = 1;}
	if (!no_input_flag && gvars->error_type == 0) {compute(arguments, operators, gvars);}
	if (stack_length(arguments) > 1 && gvars->error_type == 0) {gvars->error_type = 4;}

	gdouble return_value = 0;
	if (gvars->error_type == 0 && !no_input_flag) {stack_pop(arguments, &return_value);}
	stack_destroy(arguments);
	stack_destroy(operators);

	return return_value;
}

gdouble parse_term(const gchar *args, gint8 *_error_type, gboolean _debug)
{
	struct global_vars *gvars = malloc(sizeof(struct global_vars));
	gvars->digits = stack_init(sizeof(gint8));
	gvars->nest_level = 0;
	gvars->frac_point = 0;
	gvars->error_type = 0;
	gvars->debug = _debug;
	
	gchar *args_temp = g_strdup(args);
	if (strlen(args) == 0) {*_error_type = 5; return 0;}
	gdouble results = _parse(args_temp, gvars);
	if (gvars->nest_level != 0 && gvars->error_type == 0)
		{gvars->error_type = 1;}
	*_error_type = gvars->error_type;
	g_free(args_temp);
	stack_destroy(gvars->digits);
	g_free(gvars);
	
	if (_debug)
		{g_print("---------\n---------\n\n");}
	return results;
}
