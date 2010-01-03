#ifndef _UI_H
#define _UI_H

#include "widget.h"

void
ui_init (int, char **, char *, char *, Display **, Window *, GC *);

void
ui_event (Display *, struct widget_list *, XEvent);

#endif
