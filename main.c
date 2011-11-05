
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gtk/gtk.h"
#include "Parse.h"
#include "term_icon.xpm"

GtkWidget *window;
GtkWidget *button;
GtkWidget *label_term;
GtkWidget *label_result;
GtkWidget *entry_term;
GtkWidget *entry_result;
GtkWidget *separator;
GtkWidget *hbox1;
GtkWidget *hbox2;
GtkWidget *vbox;

GdkPixbuf *myIcon;

gboolean debugMode;

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    gtk_main_quit ();
    return FALSE;
}

static gboolean start_parsing(	GtkWidget	*widget,
								GdkEvent	*event,
								gpointer	 data )
{
	gint8 errno;
	gdouble result = parse_term
		(gtk_entry_get_text( GTK_ENTRY(entry_term) ), &errno, debugMode);
	gchar display_str[20];
	
	switch (errno)
	{
		case 0 : g_ascii_dtostr(display_str, 20, result); break;
		case 1 : memcpy(display_str, "BRACKET ERROR", 13); break;
		case 2 : memcpy(display_str, "UNKNOWN SYMBOL", 14); break;
		case 3 : memcpy(display_str, "REMOVE UNNEEDED \'.\'", 19); break;
		case 4 : memcpy(display_str, "OPERATOR ERROR", 14); break;
		case 5 : memcpy(display_str, "\0", 1); break;
		default: memcpy(display_str, "UNKNOWN ERROR", 13);
	}
	
	gtk_entry_set_text(GTK_ENTRY(entry_result), display_str);
	return FALSE;
}


int main(int argc, char *argv[])
{
	
	if (argc > 1)
		{ debugMode = (!strcmp(argv[argc-1], "--debug")); }
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "TermInterpreter");
	gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
	myIcon = gdk_pixbuf_new_from_xpm_data(term_icon_xpm);
	gtk_window_set_icon( GTK_WINDOW(window), myIcon );
	
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
	button = gtk_button_new_with_label("Berechne!");
	label_term = gtk_label_new("Term:");
	label_result = gtk_label_new("Ergebnis:");
	entry_term = gtk_entry_new();
	entry_result = gtk_entry_new();
	separator = gtk_hseparator_new();
	hbox1 = gtk_hbox_new(FALSE, 10);
	hbox2 = gtk_hbox_new(FALSE, 10);
	vbox = gtk_vbox_new(FALSE, 10);
	
	g_signal_connect (window, "delete-event",
		      G_CALLBACK (delete_event), NULL);
	g_signal_connect (	button, "clicked",
		G_CALLBACK (start_parsing), NULL);
	g_signal_connect (	entry_term, "activate",
		G_CALLBACK (start_parsing), NULL);
	
	gtk_widget_show(button);
	gtk_widget_show(label_term);
	gtk_widget_show(label_result);
	gtk_widget_show(entry_term);
	gtk_widget_show(entry_result);
	gtk_widget_show(separator);
	
	
	gtk_box_pack_start (GTK_BOX (hbox1), label_term, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox1), entry_term, TRUE, TRUE, 0);
	
	gtk_widget_show(hbox1);
	
	gtk_box_pack_start (GTK_BOX (hbox2), button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox2), label_result, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox2), entry_result, TRUE, TRUE, 0);
	
	gtk_widget_show(hbox2);
	
	gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
	
	gtk_widget_show(vbox);
	
	gtk_container_add (GTK_CONTAINER (window), vbox);
	
	gtk_widget_show(window);
	
	gtk_main();
	
	return 0;
	
	
}
