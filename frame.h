#ifndef _FRAME_H
#define _FRAME_H

#include "widget.h"

#define FRAME_FROM_WIDGET(w) ((struct frame *)((void *)(w) - \
	(void *)&((struct frame *)0)->widget))

struct frame
{
	struct widget widget;
};

int
frame_draw (struct widget *);

struct widget *
frame_create (Display *, Window, int, int, int, int, struct frame *);

#endif
