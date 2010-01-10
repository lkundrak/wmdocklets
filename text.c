#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "widget.h"
#include "text.h"

/* Create pixmap from font */
struct text_font *
text_font_load (display, window, font_data, font)
	Display *display;
	Window window;
	char *font_data[];
	struct text_font *font;
{
	font->display = display;
	font->window = window;
	font->attrib.valuemask = 0;
        XpmCreatePixmapFromData (display, window, font_data,
		&font->pixmap, NULL, &font->attrib);

	return font;
}

/* Constructor */
struct widget *
text_create (font, window, gc, x, y, width, text)
	struct text_font *font;
	Window window;
	GC gc;
	int x, y, width;
	struct text *text;
{
	text->gc = gc;
	text->font = font;
	text->width = width;
	text->widget.display = font->display;
	text->buffer = NULL;

	/* Geometry */
	text->widget.rectangle.x = x;
	text->widget.rectangle.y = y;
	text->widget.rectangle.width = width * TEXT_WIDTH(font);
	text->widget.rectangle.height = TEXT_HEIGHT(font);

	/* Window */
	text->widget.window = XCreateSimpleWindow (font->display, window,
		x, y, text->widget.rectangle.width,
		text->widget.rectangle.height, 0, 3, 0);
	XSelectInput (text->widget.display, text->widget.window,
		ButtonPressMask | ButtonReleaseMask | ExposureMask);
	XMapWindow (font->display, text->widget.window);

	/* Callbacks */	
	text->widget.draw = text_draw;
	text->widget.event = NULL;

	return &text->widget;
}

int
text_draw (widget)
	struct widget *widget;
{
	struct text *text;
	char *string;
	int x;

	text = TEXT_FROM_WIDGET(widget);
	string = text->buffer;

	if (!string)
		return 0;

	for (x = 0; *string; string++) {
		XCopyArea (widget->display, text->font->pixmap, widget->window,
			text->gc, TEXT_GLYPH(text->font, *string), x, 0);
		x += TEXT_WIDTH(text->font);
	}

	return 0;
};
