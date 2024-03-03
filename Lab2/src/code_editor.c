#include "code_editor.h"
#include "lexer.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

code_editor_t editor;

static void activate(GtkApplication *app, gpointer user_data);
static void init_window();
static void init_code();
static void init_menu();
static void init_panel();
static void init_status();
static void highlight_word();

static gboolean code_key_pressed(GtkWidget *widget, GdkEventKey *event);
static gboolean code_key_released(GtkWidget *widget, GdkEventKey *event);
static void code_scrolled(GtkWidget *widget, GdkEvent *event);
static void code_text_buffer_changed(GtkWidget *widget, GdkEventKey *event);

static void new_doc_pressed(GtkWidget *widget, gpointer data);
static void open_doc_pressed(GtkWidget *widget, gpointer data);
static void save_doc_pressed(GtkWidget *widget, gpointer data);
static void save_as_doc_pressed(GtkWidget *widget, gpointer data);
static void undo_pressed (GtkWidget *widget, gpointer data);
static void redo_pressed (GtkWidget *widget, gpointer data);
static void cut_pressed (GtkWidget *widget, gpointer data);
static void copy_pressed (GtkWidget *widget, gpointer data);
static void paste_pressed (GtkWidget *widget, gpointer data);
static void delete_pressed (GtkWidget *widget, gpointer data);
static void select_all_pressed (GtkWidget *widget, gpointer data);
static void about_pressed (GtkWidget *widget, gpointer data);
static void faq_pressed (GtkWidget *widget, gpointer data);

void destroy (GtkWidget* widget, gpointer data);

static void activate(GtkApplication *app, gpointer user_data)
{
	editor.builder = gtk_builder_new();
	gtk_builder_add_from_file(editor.builder, "src/main_window_copy.ui", NULL);
	
	GObject *obj = gtk_builder_get_object(editor.builder, "quit_doc");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(destroy), app);
	
	init_window();

	gtk_window_set_application(GTK_WINDOW(editor.window), app);
	gtk_widget_set_visible(GTK_WIDGET(editor.window), TRUE);
}

static void init_window()
{
	editor.window = gtk_builder_get_object(editor.builder, "window");
	init_code();
	init_menu();
	init_panel();
	init_status();
}

static void init_code()
{
	editor.code_view.text_view = gtk_builder_get_object(editor.builder, "code");
	editor.code_view.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor.code_view.text_view));
	
	editor.analysis_view.text_view = gtk_builder_get_object(editor.builder, "analysis");
	editor.analysis_view.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor.analysis_view.text_view));
	
	for (int i = 0; i < LEXEME_COLORS_COUNT; i++)
	{
		GdkRGBA color;
		gdk_rgba_parse(&color, colors[i].color);
		gtk_text_buffer_create_tag(editor.code_view.buffer, colors[i].lexeme, "foreground-rgba", &color, NULL);
	}

	for (int i = 0; i < DYNAMIC_FONTS_COUNT; i++)
	{
		gtk_text_buffer_create_tag(editor.code_view.buffer, fonts[i], "font", fonts[i], NULL);
		//gtk_text_buffer_create_tag(editor.analysis_view.buffer, fonts[i], "font", fonts[i], NULL);
	}

	g_signal_connect(GTK_TEXT_VIEW(editor.code_view.text_view), "key-release-event", G_CALLBACK(code_key_released), NULL);
	g_signal_connect(GTK_TEXT_VIEW(editor.code_view.text_view), "key-press-event", G_CALLBACK(code_key_pressed), NULL);
	g_signal_connect(editor.code_view.text_view, "scroll-event", G_CALLBACK(code_scrolled), NULL);
	g_signal_connect(GTK_TEXT_BUFFER(editor.code_view.buffer), "changed", G_CALLBACK(code_text_buffer_changed), NULL);
}

static void init_menu()
{
	GObject *obj = gtk_builder_get_object(editor.builder, "new_doc");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(new_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "open_doc");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(open_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "save_doc");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(save_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "save_as_doc");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(save_as_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "undo");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(undo_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "redo");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(redo_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "cut");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(cut_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "copy");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(copy_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "paste");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(paste_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "delete");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(delete_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "select_all");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(select_all_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "about");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(about_pressed), NULL);
	
	obj = gtk_builder_get_object(editor.builder, "faq");
	g_signal_connect(GTK_MENU_ITEM(obj), "activate", G_CALLBACK(faq_pressed), NULL);
}

static void init_panel()
{
	GObject *obj = gtk_builder_get_object(editor.builder, "new_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(new_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "open_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(open_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "save_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(save_doc_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "undo_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(undo_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "redo_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(redo_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "cut_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(cut_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "copy_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(copy_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "paste_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(paste_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "faq_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(faq_pressed), NULL);

	obj = gtk_builder_get_object(editor.builder, "about_panel");
	g_signal_connect(GTK_BUTTON(obj), "clicked", G_CALLBACK(about_pressed), NULL);	
}

static void init_status()
{

	editor.status = gtk_builder_get_object(editor.builder, "status");
}


static void undo_pressed (GtkWidget *widget, gpointer data)
{
	current_buffer_pos -= 1;
	//g_print("Undoed: %s, strlen=%d\n", editor.arr[current_buffer_pos], strlen(editor.arr[current_buffer_pos]));
	gtk_text_buffer_set_text(editor.code_view.buffer, editor.arr[current_buffer_pos], strlen(editor.arr[current_buffer_pos]));
}

static void redo_pressed (GtkWidget *widget, gpointer data)
{

}

static void cut_pressed (GtkWidget *widget, gpointer data)
{
	copy_pressed(widget, data);
	delete_pressed(widget, data);
}

static void copy_pressed (GtkWidget *widget, gpointer data)
{
	GtkTextIter start, end;
	gtk_text_buffer_get_selection_bounds(editor.code_view.buffer, &start, &end);
	editor.copied_text = gtk_text_buffer_get_text(editor.code_view.buffer, &start, &end, TRUE);
}

static void paste_pressed (GtkWidget *widget, gpointer data)
{
	if (editor.copied_text != "")
		gtk_text_buffer_insert_at_cursor(editor.code_view.buffer, editor.copied_text, strlen(editor.copied_text));
}

static void delete_pressed (GtkWidget *widget, gpointer data)
{
	gtk_text_buffer_delete_selection(editor.code_view.buffer, TRUE, TRUE);
}
static void select_all_pressed (GtkWidget *widget, gpointer data)
{
	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	gtk_text_buffer_select_range(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
}

static void about_pressed (GtkWidget *widget, gpointer data)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "src/about.ui", NULL);
	GObject *obj = gtk_builder_get_object(builder, "window");
	gtk_widget_show(GTK_WIDGET(obj));
}

static void faq_pressed (GtkWidget *widget, gpointer data)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "src/faq.ui", NULL);
	GObject *obj = gtk_builder_get_object(builder, "window");
	gtk_widget_show(GTK_WIDGET(obj));
}

static gboolean code_key_pressed(GtkWidget *widget, GdkEventKey *event)
{
	switch (event->keyval)
	{
		case GDK_KEY_Control_L:
			MODIFIER_ACTION(LCONTROL, 1);
			break;
		case GDK_KEY_Control_R:
			MODIFIER_ACTION(RCONTROL, 1);
			break;
		case GDK_KEY_Shift_L:
			MODIFIER_ACTION(LSHIFT, 1);
			break;
		case GDK_KEY_Shift_R:
			MODIFIER_ACTION(RSHIFT, 1);
			break;
		case GDK_KEY_Alt_L:
			MODIFIER_ACTION(LALT, 1);
			break;
		case GDK_KEY_Alt_R:
			MODIFIER_ACTION(RALT, 1);
	}

	return FALSE;
}


static void code_text_buffer_changed(GtkWidget *widget, GdkEventKey *event)
{

	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	gtk_text_buffer_apply_tag_by_name(editor.code_view.buffer, fonts[current_font], &editor.code_view.start, &editor.code_view.end);
	char *text = gtk_text_buffer_get_text(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end, TRUE);
	
/*
	editor.arr[current_buffer_pos] = text;

	if (current_buffer_pos != BUFFER_LOGS_COUNT)
		current_buffer_pos++;
	else
		for (int i = 0; i < current_buffer_pos; i++)
			editor.arr[i] = editor.arr[i + 1];
			*/
/*
	g_print("Buffer pos=%d\n", current_buffer_pos);
	for (int i = 0; i < BUFFER_LOGS_COUNT; i++)
		g_print("Buffer[%d]=%s\n", i, editor.arr[i]);
	editor.not_saved = 1;
	highlight_word();
*/
	char *token = expr_to_tokens_str(text);


	//gtk_text_buffer_get_bounds(editor.analysis_view.buffer, &editor.analysis_view.start, &editor.analysis_view.end);
	if (token != NULL)
	{
		gtk_text_buffer_set_text(editor.analysis_view.buffer, token, strlen(token));
	//gtk_text_buffer_apply_tag_by_name(editor.analysis_view.buffer, fonts[2], &editor.analysis_view.start, &editor.analysis_view.end);

	}
	else gtk_text_buffer_set_text(editor.analysis_view.buffer, "", strlen(""));
}

static gboolean code_key_released(GtkWidget *widget, GdkEventKey *event)
{
	switch (event->keyval)
	{
		case GDK_KEY_Control_L:
			MODIFIER_ACTION(LCONTROL, 0);
			break;
		case GDK_KEY_Control_R:
			MODIFIER_ACTION(RCONTROL, 0);
			break;
		case GDK_KEY_Shift_L:
			MODIFIER_ACTION(LSHIFT, 0);
			break;
		case GDK_KEY_Shift_R:
			MODIFIER_ACTION(RSHIFT, 0);
			break;
		case GDK_KEY_Alt_L:
			MODIFIER_ACTION(LALT, 0);
			break;
		case GDK_KEY_Alt_R:
			MODIFIER_ACTION(RALT, 0);
	}
	//g_print("Text entered\n");
	return FALSE;
}

static void code_scrolled(GtkWidget *widget, GdkEvent *event)
{
	GdkEventScroll *scroll_event = (GdkEventScroll *)event;
	if(code_editor_text_view_modifier_condition & (LCONTROL | RCONTROL))
	{
		editor.code_view.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor.code_view.text_view));
		gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
		if (scroll_event->delta_y < 0.0)
		{
			//g_print("Increase, cur=%d\n", current_font);
			current_font += current_font == DYNAMIC_FONTS_COUNT - 1 ? 0 : 1;
			gtk_text_buffer_apply_tag_by_name(editor.code_view.buffer, fonts[current_font], &editor.code_view.start, &editor.code_view.end);
		}
		if (scroll_event->delta_y > 0.0)
		{
			//g_print("Decrease, cur=%d\n", current_font);
			gtk_text_buffer_remove_tag_by_name(editor.code_view.buffer, fonts[current_font], &editor.code_view.start, &editor.code_view.end);
			current_font -= current_font == 0 ? 0 : 1;
		}
	}
}

static void highlight_word()
{
	GtkTextIter match_start, match_end;
	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	const gchar *word;
	for (int i = 0; i < LEXEME_COLORS_COUNT; i++)
	{
		while (gtk_text_iter_forward_search(&editor.code_view.start, colors[i].lexeme, GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL)) 
		{
        	gtk_text_buffer_apply_tag_by_name(editor.code_view.buffer, colors[i].lexeme, &match_start, &match_end);
        	gtk_text_iter_forward_char(&match_end);
        	editor.code_view.start = match_end;
    	}
	}

}

static void new_doc_pressed(GtkWidget *widget, gpointer data)
{
	save_doc_pressed(widget, data);

	if (editor.fp)
	{
		fclose(editor.fp);
		gtk_statusbar_pop(GTK_STATUSBAR(editor.status), 1);
	}
	editor.not_saved = 0;
	gtk_text_buffer_set_text(editor.code_view.buffer, "", 0);
	//g_print("New doc created!\n");
}

char  *get_filename(FILE *f) {
	int fd;
	char fd_path[255];
	char *filename = malloc(255);
	ssize_t n;

	fd = fileno(f);
	sprintf(fd_path, "/proc/self/fd/%d", fd);
	n = readlink(fd_path, filename, 255);
	if (n < 0)
		return NULL;
	filename[n] = '\0';
	return filename;
}

static void open_doc_pressed(GtkWidget *widget, gpointer data)
{

	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	char *text = gtk_text_buffer_get_text(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end, TRUE);

	save_doc_pressed(widget, data);

	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open File",
	                                      GTK_WINDOW(editor.window),
	                                      action,
	                                      ("_Cancel"),
	                                      GTK_RESPONSE_CANCEL,
	                                      ("_Open"),
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);
	chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		editor.fp = fopen(gtk_file_chooser_get_filename (chooser), "r");
		fseek(editor.fp, 0, SEEK_END);
		unsigned size = ftell(editor.fp);
		fseek(editor.fp, 0, SEEK_SET);

		char *text = malloc(size);
		fread(text, 1, size, editor.fp);

		gtk_text_buffer_set_text(editor.code_view.buffer, text, size);
		gtk_statusbar_push(GTK_STATUSBAR(editor.status), 1, get_filename(editor.fp));
	}

	gtk_widget_destroy (dialog);
}

static void save_doc_pressed(GtkWidget *widget, gpointer data)
{
	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	char *text = gtk_text_buffer_get_text(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end, TRUE);

	if (editor.not_saved && editor.fp)
	{
		fprintf(editor.fp, text);
		editor.not_saved = 0;
		gtk_statusbar_push(GTK_STATUSBAR(editor.status), 1, get_filename(editor.fp));
	}
	else if (editor.not_saved && !editor.fp)
	{
		save_as_doc_pressed(widget, data);
	}
	else return;
	gtk_statusbar_pop(GTK_STATUSBAR(editor.status), 1);
	return;
}

static void save_as_doc_pressed(GtkWidget *widget, gpointer data)
{

	gtk_text_buffer_get_bounds(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end);
	char *text = gtk_text_buffer_get_text(editor.code_view.buffer, &editor.code_view.start, &editor.code_view.end, TRUE);

	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Save File",
                                      GTK_WINDOW(editor.window),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Save"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
	chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		editor.fp = fopen(gtk_file_chooser_get_filename (chooser), "w+");
		fprintf(editor.fp, text);
		editor.not_saved = 0;
		gtk_statusbar_push(GTK_STATUSBAR(editor.status), 1, get_filename(editor.fp));
		printf(get_filename(editor.fp));
	}
	gtk_widget_destroy (dialog);

}

void destroy (GtkWidget* widget, gpointer data)
{
	if (editor.not_saved)
		save_as_doc_pressed(widget, data);
    g_application_quit(G_APPLICATION(data));
}

int main(int argc, char **argv)
{
	#ifdef GTK_SRCDIR
		g_chdir (GTK_SRCDIR);
	#endif
	GtkApplication *app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK(activate), app);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	g_object_unref(editor.builder);
	return status;
}

