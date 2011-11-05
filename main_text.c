//      main.c
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




#include "Parse.h"

#include <stdio.h>


int main(void)
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
	printf("Results!!: %f\n", parse_term(hi, &t, TRUE));
	//printf("= %f\n", parse_term(hi, &t, FALSE));
	printf("Errors... : %d\n", t);
	return 0;
}
