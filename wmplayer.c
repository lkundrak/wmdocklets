#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <pthread.h>

#include "ui.h"
#include "text.h"
#include "button.h"
#include "frame.h"

#include "player.h"

#include "font/digit-small.xpm"
#include "font/digit.xpm"

#include "controls/previous.xpm"
#include "controls/play.xpm"
#include "controls/next.xpm"
#include "controls/stop.xpm"
#include "controls/eject.xpm"

/* Changable from backend */
struct text artist, title;
struct text track, elapsed;

/* Don't race with xlib */
pthread_mutex_t draw_mutex;

int
main (argc, argv)
	int argc;
	char *argv[];
{
	Display *display;
	Window icon;
	GC gc;

	struct widget_list *widgets = NULL;
	struct frame button_frame;
	struct text_font digit_small, digit;
	struct frame artist_frame, title_frame;
	struct frame track_frame, elapsed_frame;
	struct button previous, play, next, stop, eject;

	pthread_t player_thread;

	ui_init (argc, argv, "chuck", "Chuck", &display, &icon, &gc);

	/* Button bar */
	widget_add (&widgets, button_create (display, icon, gc,
		 1, 45, 11, 11, previous_track, &previous));
	widget_add (&widgets, button_create (display, icon, gc,
		 12, 45, 11, 11, play_pause, &play));
	widget_add (&widgets, button_create (display, icon, gc,
		 23, 45, 11, 11, next_track, &next));
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

	/* Text displays */
	text_font_load (display, icon, digit_xpm, &digit);
	text_font_load (display, icon, digit_small_xpm, &digit_small);

	widget_add (&widgets, text_create (&digit, icon, gc,
		4, 4, 2, &track));
	widget_add (&widgets, frame_create (display, icon,
		track.widget.rectangle.x - 3,
		track.widget.rectangle.y - 3,
		track.widget.rectangle.width + 6,
		track.widget.rectangle.height + 6,
		&track_frame));

	widget_add (&widgets, text_create (&digit, icon, gc,
		26, 4, 4, &elapsed));
	widget_add (&widgets, frame_create (display, icon,
		elapsed.widget.rectangle.x - 3,
		elapsed.widget.rectangle.y - 3,
		elapsed.widget.rectangle.width + 6,
		elapsed.widget.rectangle.height + 6,
		&elapsed_frame));

	widget_add (&widgets, text_create (&digit_small, icon, gc,
		2, 20, 9, &artist));
	widget_add (&widgets, frame_create (display, icon,
		artist.widget.rectangle.x - 1,
		artist.widget.rectangle.y - 1,
		artist.widget.rectangle.width + 2,
		artist.widget.rectangle.height + 2,
		&artist_frame));

	widget_add (&widgets, text_create (&digit_small, icon, gc,
		2, 33, 9, &title));
	widget_add (&widgets, frame_create (display, icon,
		title.widget.rectangle.x - 1,
		title.widget.rectangle.y - 1,
		title.widget.rectangle.width + 2,
		title.widget.rectangle.height + 2,
		&title_frame));

	/* The rest is transparent */
	widget_mask (widgets, icon);

	/* Spawn the music player back end */
	pthread_mutex_init (&draw_mutex, NULL);
	pthread_create (&player_thread, NULL, player, NULL);

	while (1) {
		XEvent event;
		XNextEvent (display, &event);
		pthread_mutex_lock (&draw_mutex);
		ui_event (display, widgets, event);

		/* FIXME: Only flush if the event queue is empty */
		XFlush (event.xany.display);
		pthread_mutex_unlock (&draw_mutex);
	}

	return 0;
}
