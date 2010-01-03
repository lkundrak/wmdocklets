#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "widget.h"
#include "button.h"

int
button_event (widget, event)
	struct widget *widget;
	XEvent event;
{
	struct button *button;

	button = BUTTON_FROM_WIDGET(widget);
	button->pushed = !button->pushed;
	widget_draw (widget);

	if (button->action)
		button->action (widget);

	return 0;
}

struct widget *
button_create (display, window, gc, x, y, width, height, action, button)
	Display *display;
	Window window;
	GC gc;
	int x, y, width, height;
	widget_callback action;
	struct button *button;
{
	button->pushed = 0;
	button->pixmap = 0;
	button->widget.display = display;

	/* Geometry */
	button->widget.rectangle.x = x;
	button->widget.rectangle.y = y;
	button->widget.rectangle.width = width;
	button->widget.rectangle.height = height;

	/* Window */
	button->widget.window = XCreateSimpleWindow (display, window,
		x, y, width, height, 0, 3, 0);
	XSelectInput (display, button->widget.window,
		ButtonPressMask | ButtonReleaseMask | ExposureMask);
	XMapWindow (display, button->widget.window);

	/* Callbacks */
	button->widget.draw = button_draw;
	button->widget.event = button_event;
	button->action = action;

	return &button->widget;
}

int
button_draw (widget)
	struct widget *widget;
{
	struct button *button;
	static GC gc = 0;
	static XColor light, dark, fill;
	Display *display;
	Window window;
	int width, height;

	button = BUTTON_FROM_WIDGET(widget);

	/* Shortcuts */
	display = widget->display;
	window = widget->window;
	width = widget->rectangle.width;
	height = widget->rectangle.height;

	/* Initialize the static shit for the first time */
	if (!gc) {
		static Colormap colormap;

		gc = XCreateGC (display, window, 0, NULL);
		colormap = DefaultColormap (display,
			DefaultScreen (display));
		XParseColor(display, colormap, "#cccccc", &fill);
		XAllocColor(display, colormap, &fill);
		XParseColor(display, colormap, "#000000", &dark);
		XAllocColor(display, colormap, &dark);
		XParseColor(display, colormap, "#f6f2ff", &light);
		XAllocColor(display, colormap, &light);
	}


	/* Fill */
	if (button->pixmap) {
		XCopyArea (display, button->pixmap, window, gc,
			0, 0, width - 1, height, 1, 1);
	}

	/* Top left */
	XSetForeground (display, gc, (button->pushed ? dark : light).pixel);
	XDrawLine (display, window, gc, 0, 0, width - 1, 0);
	XDrawLine (display, window, gc, 0, 0, 0, height - 1);

	/* Bottom right */
	XSetForeground (display, gc, (button->pushed ? light : dark).pixel);
	XDrawLine (display, window, gc, width - 1, 1, width - 1, height - 1);
	XDrawLine (display, window, gc, 1, height - 1, width - 1, height - 1);

	return 0;
}
