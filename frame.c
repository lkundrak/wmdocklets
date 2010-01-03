#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "widget.h"
#include "frame.h"

struct widget *
frame_create (display, window, x, y, width, height, frame)
	Display *display;
	Window window;
	int x, y, width, height;
	struct frame *frame;
{
	frame->widget.display = display;

	/* Geometry */
	frame->widget.rectangle.x = x;
	frame->widget.rectangle.y = y;
	frame->widget.rectangle.width = width;
	frame->widget.rectangle.height = height;

	/* Window */
	frame->widget.window = XCreateSimpleWindow (display, window,
		x, y, width, height, 0, 3, 0);
	XSelectInput (display, frame->widget.window, ExposureMask);
	XLowerWindow(display, frame->widget.window);
	XMapWindow (display, frame->widget.window);

	/* Callbacks */
	frame->widget.draw = frame_draw;
	frame->widget.event = NULL;

	return &frame->widget;
}

int
frame_draw (widget)
	struct widget *widget;
{
	struct frame *frame;
	static GC gc = 0;
	static XColor light, dark, fill;
	Display *display;
	Window window;
	int width, height;

	frame = FRAME_FROM_WIDGET(widget);

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

	/* Top left */
	XSetForeground (display, gc, dark.pixel);
	XDrawLine (display, window, gc, 0, 0, width - 1, 0);
	XDrawLine (display, window, gc, 0, 0, 0, height - 1);

	/* Bottom right */
	XSetForeground (display, gc, light.pixel);
	XDrawLine (display, window, gc, width - 1, 1, width - 1, height - 1);
	XDrawLine (display, window, gc, 1, height - 1, width - 1, height - 1);

	return 0;
}
