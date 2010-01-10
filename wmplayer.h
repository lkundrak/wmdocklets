#ifndef _PEW_H
#define _PEW_H

#include "text.h"

extern struct text artist, title;
extern struct text track, elapsed;

extern pthread_mutex_t draw_mutex;

#endif
