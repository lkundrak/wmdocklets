#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#include "widget.h"

int
widget_draw (widget)
	struct widget *widget;
{
	return widget->draw (widget);
}

/* Register a widget in the list */
void
widget_add (widgets, widget)
	struct widget_list **widgets;
	struct widget *widget;
{
	struct widget_list **iter;

	for (iter = widgets; *iter; iter = &(*iter)->next);
	*iter = malloc (sizeof (struct widget_list));
	(*iter)->next = NULL;
	(*iter)->widget = widget;
}

/* Return the widget structure for a window */
struct widget *
widget_window (widgets, window)
	struct widget_list *widgets;
	Window window;
{
	struct widget_list *iter;

	for (iter = widgets; iter; iter = iter->next) {
		if (iter->widget->window == window)
			return iter->widget;
	}

	return NULL;
}

/* Let the widget handle the event if it wants to */
int
widget_event (widget, event)
	struct widget *widget;
	XEvent event;
{
	if (widget->event)
		return widget->event (widget, event);
	return 0;
}

/* Shape the window for its subwindows */
void
widget_mask (widgets, window)
	struct widget_list *widgets;
	Window window;
{
	struct widget_list *iter;

	for (iter = widgets; iter; iter = iter->next) {
                XShapeCombineRectangles (iter->widget->display, window,
			ShapeBounding, 0, 0, &iter->widget->rectangle, 1,
			iter == widgets ? ShapeSet : ShapeUnion, 0);
        }
}
