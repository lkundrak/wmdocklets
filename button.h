#ifndef _BUTTON_H
#define _BUTTON_H

#include "widget.h"

#define BUTTON_FROM_WIDGET(w) ((struct button *)((void *)(w) - \
	(void *)&((struct button *)0)->widget))

struct button
{
	int pushed;
	struct widget widget;
	Pixmap picture;
	widget_callback *action; 
};

int
button_draw (struct widget *);

struct widget *
button_create (Display *, Window, int, int, int, int, widget_callback *,
	struct button *);

#endif
