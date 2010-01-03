#ifndef _TEXT_H
#define _TEXT_H

#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "widget.h"

#define TEXT_FROM_WIDGET(w) ((struct text *)((void *)(w) - \
        (void *)&((struct text *)0)->widget))

#define TEXT_WIDTH(font) (font->attrib.width / 0x100)
#define TEXT_HEIGHT(font) (font->attrib.height)
#define TEXT_GLYPH(font, glyph) (glyph * TEXT_WIDTH(font)), 0, \
	TEXT_WIDTH(font), TEXT_HEIGHT(font)

struct text_font {
	Pixmap pixmap;
	XpmAttributes attrib;
	Display *display;
	Window window;
};

struct text {
	struct text_font *font;
	struct widget widget;
	GC gc;
	int width;
	char *buffer;
};

struct text_font *
text_font_load (Display *, Window, char **, struct text_font *);

int
text_draw (struct widget *);

struct widget *
text_create (struct text_font *, Window, GC, int, int, int, struct text *);

#endif
