#ifndef _WIDGET_H
#define _WIDGET_H

#include <X11/Xlib.h>

struct widget;
typedef int widget_callback(struct widget *);
typedef int widget_event_callback(struct widget *, XEvent);

struct widget
{
	Display *display;
	Window window;
	widget_callback *draw;
	widget_event_callback *event;
	XRectangle rectangle;
};

struct widget_list
{
	struct widget *widget;
	struct widget_list *next;
};

int
widget_draw (struct widget *);

void
widget_add (struct widget_list **, struct widget *);

struct widget *
widget_window (struct widget_list *, Window);

int
widget_event (struct widget *, XEvent);

void
widget_mask (struct widget_list *, Window);

#endif
