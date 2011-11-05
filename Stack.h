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


#include <glib.h>


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

struct stack* stack_init(gint content_size);

void stack_push(struct stack *lstack, gpointer push_content);

void stack_pop(struct stack *lstack, gpointer return_location);

gint stack_length(struct stack *lstack);

void stack_destroy(struct stack *lstack);

void stack_dump(struct stack *lstack, gchar mode);
