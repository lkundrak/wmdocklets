#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <stdio.h>

#include "ui.h"
#include "text.h"
#include "button.h"
#include "frame.h"

#include "font/digit-small.xpm"

#include "controls/previous.xpm"
#include "controls/play.xpm"
#include "controls/next.xpm"
#include "controls/stop.xpm"
#include "controls/eject.xpm"

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
	struct button previous, play, next, stop, eject;
	struct frame button_frame, frame2;
	struct text_font font;
	struct text text1;

	ui_init (argc, argv, "chuck", "Chuck", &display, &icon, &gc);

	/* Button bar */
	widget_add (&widgets, button_create (display, icon, gc,
		 1, 45, 11, 11, NULL, &previous));
	widget_add (&widgets, button_create (display, icon, gc,
		 12, 45, 11, 11, NULL, &play));
	widget_add (&widgets, button_create (display, icon, gc,
		 23, 45, 11, 11, NULL, &next));
	widget_add (&widgets, button_create (display, icon, gc,
		 34, 45, 11, 11, NULL, &stop));
	widget_add (&widgets, button_create (display, icon, gc,
		 45, 45, 11, 11, NULL, &eject));
	widget_add (&widgets, frame_create (display, icon,
		previous.widget.rectangle.x - 1,
		previous.widget.rectangle.y - 1,
		previous.widget.rectangle.width * 5 + 2,
		previous.widget.rectangle.height + 2,
		&button_frame));

	/* Icons */
	XpmCreatePixmapFromData (display, icon, previous_xpm,
		&previous.pixmap, NULL, NULL);
	XpmCreatePixmapFromData (display, icon, play_xpm,
		&play.pixmap, NULL, NULL);
	XpmCreatePixmapFromData (display, icon, next_xpm,
		&next.pixmap, NULL, NULL);
	XpmCreatePixmapFromData (display, icon, stop_xpm,
		&stop.pixmap, NULL, NULL);
	XpmCreatePixmapFromData (display, icon, eject_xpm,
		&eject.pixmap, NULL, NULL);

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
