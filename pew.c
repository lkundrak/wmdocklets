#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <stdio.h>

#include "ui.h"
#include "text.h"
#include "button.h"
#include "frame.h"

#include "font/digit-small.xpm"

int
meow (widget)
	struct widget *widget;
{
	printf ("japierdole\n");
	return 0;
}

int
main (argc, argv)
	int argc;
	char *argv[];
{
	Display *display;
	Window icon;
	GC gc;

	struct widget_list *widgets = NULL;
	struct button button1, button2;
	struct frame frame1, frame2;
	struct text_font font;
	struct text text1;

	ui_init (argc, argv, "chuck", "Chuck", &display, &icon, &gc);

	/* Button bar */
	widget_add (&widgets, frame_create (display, icon,
		9, 9, 32, 12, &frame1));
	widget_add (&widgets, button_create (display, icon, 10, 10, 10, 10,
		NULL, &button1));
	widget_add (&widgets, button_create (display, icon, 20, 10, 20,
		10, NULL, &button2));

	/* Test display */
	text_font_load (display, icon, digit_small_xpm, &font);
	widget_add (&widgets, text_create (&font, icon, gc, 2, 30, 9, &text1));
	widget_add (&widgets, frame_create (display, icon,
		text1.widget.rectangle.x - 1,
		text1.widget.rectangle.y - 1,
		text1.widget.rectangle.width + 2,
		text1.widget.rectangle.height + 2,
		&frame2));

	/* The rest is transparent */
	widget_mask (widgets, icon);

	while (1) {
		XEvent event;
		XNextEvent (display, &event);
		ui_event (display, widgets, event);
	}

	return 0;
}
