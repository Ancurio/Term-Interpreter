//      Stacks.c
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

#include <glib.h>
#include <glib/gprintf.h>


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

