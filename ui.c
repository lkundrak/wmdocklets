#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "widget.h"

void
ui_init (argc, argv, name, class, display, window, gc)
	int argc;
	char *argv[];
	char *name, *class;
	Display **display;
	Window *window;
	GC *gc;
{
	Window root, mainwin;
	XWMHints *hints;
	XClassHint *classhint;
	Colormap colormap;
	XColor color;

	*display = XOpenDisplay ("");

	root = DefaultRootWindow (*display);
	*gc = XCreateGC (*display, root, 0, NULL);
	colormap = DefaultColormap (*display, DefaultScreen (*display));

	XParseColor(*display, colormap, "Cadet Blue", &color);
	XAllocColor(*display, colormap, &color);

	/* Create both the icon window and it's bogus parent */
	mainwin = XCreateSimpleWindow (*display, root, 0, 0, 57, 57,
		0, 3, color.pixel);
	*window = XCreateSimpleWindow (*display, mainwin, 0, 0, 57, 57,
		0, 3, color.pixel);
	XMapWindow (*display, mainwin);
	XSelectInput (*display, *window, ExposureMask);

	/* Set class hints, WM would ignore us otherwise */
	classhint = XAllocClassHint ();
	classhint->res_name = name;
	classhint->res_class = class;
	XSetClassHint (*display, mainwin, classhint);

	/* Set up icon window as dock icon */
	hints = XAllocWMHints ();
	hints->flags = StateHint | IconWindowHint | WindowGroupHint;
	hints->initial_state = WithdrawnState;
	hints->icon_window = *window;
	hints->window_group = mainwin;
	XSetWMHints (*display, mainwin, hints);
}

/* Dispatch event to approriate widget */
void
ui_event (display, widgets, event)
	Display *display;
	struct widget_list *widgets;
	XEvent event;
{
	struct widget *widget;

	switch (event.type) {
	case GraphicsExpose:
	case NoExpose:
		/* Ignore these */
		break;
	case Expose:
		widget = widget_window (widgets, event.xexpose.window);
		if (widget)
			widget_draw (widget);
		break;
	default:
		widget = widget_window (widgets, event.xany.window);
		if (widget)
			widget_event (widget, event);
		break;
	}
}
