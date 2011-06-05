//      Utility.h
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

#include <glib.h>


struct global_vars
{
	struct stack *digits;
	gint nest_level;
	gint8 frac_point;
	gint8 error_type;
	gboolean debug;
};

/** array wrapper */
gchar char_at(gchar *string, gint i);

/** check if char represents an operator */
gboolean isoperator(gchar c);

/** determin priority from operator */
gint8 priority(gchar c);

/** carry out primitive arithmetics */
gdouble calculate(gdouble a, gdouble b, gchar op);

/** compute until operator stack is empty (result is stored in arguments stack) */
void compute(struct stack *arguments, struct stack *operators, struct global_vars *gvars);

/** convert char to double */
gdouble to_d(char c);

/** join digits to one int */
gdouble join_digits(struct stack *digits, gint8 frac_point);


gdouble compute_function(gchar *f_name, gdouble arg, struct global_vars *gvars);

gdouble lookup_constant(gchar *c_name, struct global_vars *gvars);
