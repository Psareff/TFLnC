#include <gtk/gtk.h>

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

/*
 * Structures reflecting XML UI on GTK backend
 */

typedef struct code_editor_text_view
{
	GObject *text_view;
	GtkTextBuffer *buffer;
	GtkTextIter start,
	            end;
} code_editor_text_view_t;

#define BUFFER_LOGS_COUNT 10
typedef struct code_editor
{
	GtkBuilder *builder;
	GObject *window;
	GObject *status;
	FILE *fp;
	char not_saved;
	code_editor_text_view_t code_view;
	code_editor_text_view_t analysis_view;
	char *arr[BUFFER_LOGS_COUNT];
	char *copied_text;
} code_editor_t;

int current_buffer_pos = 0;

/*
 * dynamic system of fonts, used to enhance visibility of small fonts
 */

#define DYNAMIC_FONTS_COUNT 7
typedef char *dynamic_font;
int current_font = 3;

dynamic_font fonts[DYNAMIC_FONTS_COUNT] =
{
	"Liberation Mono 10",
	"Liberation Mono 12",
	"Liberation Mono 14",
	"Liberation Mono 16",
	"Liberation Mono 18",
	"Liberation Mono 20",
	"Liberation Mono 22"
};

/*
 * system of lexeme colorization
 */

#define LEXEME_COLORS_COUNT 12
typedef struct lexeme_color
{
	const char *color;
	const char *lexeme;
} lexeme_color_t;

const lexeme_color_t colors[LEXEME_COLORS_COUNT] = 
{
	{"#6895d2", "typedef"},
	{"#6895d2", "int"},
	{"#6895d2", "double"},
	{"#6895d2", "char"},
	{"#6895d2", "unsigned"},
	{"#6895d2", "static"},

	{"#96E9C6", "#define"},
	{"#96E9C6", "#include"},
	{"#96E9C6", "#ifdef"},
	{"#96E9C6", "#endif"},

	{"#FF6868", "struct"},
	{"#FF6868", "class"},
};

/*
 * system of flexible processing of modifier keys
 */

#define LSHIFT   (1 << 0)
#define LCONTROL (1 << 1)
#define LALT     (1 << 2)
#define RSHIFT   (1 << 3)
#define RCONTROL (1 << 4)
#define RALT     (1 << 5)

char code_editor_text_view_modifier_condition = 0x0;

#define MODIFIER_ACTION(modifier, action) \
{ \
	if (action) code_editor_text_view_modifier_condition |= modifier; \
	else code_editor_text_view_modifier_condition &= !modifier; \
}

#endif // MAIN_WINDOW_H_
