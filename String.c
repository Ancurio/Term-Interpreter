#include <glib.h>

gint string_length(gchar *string)
{
	gint i; for (i=0;string[i] != '\0';i++) {}
	return i;
}

void string_concat(gchar *string, gchar c)
{
	gint l = string_length(string);
	string[l] = c; string [l+1] = 0;
}

void string_clear(gchar *string)
{
	gint i, l;
	l = string_length(string);
	for (i=0;i<l;i++) {string[i] = 0;}
}
