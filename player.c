#include <dbus/dbus-glib.h>
#include <id3tag.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "widget.h"

#include "player.h"
#include "pew.h"

static char elapsed_text[5];
static char track_text[2];
static char *title_text = NULL;
static char *artist_text = NULL;

DBusGProxy *proxy;

char *file_uri (uri)
	char *uri;
{
	char *src, *dst;

	src = dst = uri;

	if (strstr (src, "file://") == src)
		src += 7;

	while (*src) {
		/* Escaped */
		if (src[0] == '%' && isxdigit(src[1])
			&& isxdigit(src[2])) {
			src++;
			*dst = isdigit (*src) ? *src - '0'
				: 0xa + tolower(*src) - 'a';
			*dst *= 0x10;
			src++;
			*dst += isdigit (*src) ? *src - '0'
				: 0xa + tolower(*src) - 'a';
			src++;
			dst++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';

	return uri;
}

char *
get_id3str (file, frame_name)
	struct id3_file *file;
	char const *frame_name;
{
	struct id3_tag *tag;
	struct id3_frame *frame;
	union id3_field *field;
	tag = id3_file_tag (file);

	frame = id3_tag_findframe (tag, frame_name, 0);
	if (!frame)
		return NULL;

	/* 0th field is encoding we don't care about */
	field = id3_frame_field (frame, 1);
	if (!field)
		return NULL;

	switch (field->immediate.type) {
	case ID3_FIELD_TYPE_STRINGLIST:
		if (field->stringlist.nstrings == 0)
			return NULL;
		return (char *)id3_ucs4_latin1duplicate
			(field->stringlist.strings[0]);
	default:
		return NULL;
	}
}

/* Signal callbacks */

void
elapsedChanged (proxy, secs, data)
	DBusGProxy *proxy;
	guint32 secs;
	/* FIXME: pass elapsed widget here */
	void *data;
{
	if (secs >= 600) {
		sprintf (elapsed_text, "-%4i", secs);
	} else {
		sprintf (elapsed_text, "%1i:%02i", secs / 60, secs % 60);
	}

	/* FIXME: This could be done only once */
	elapsed.buffer = elapsed_text;

	/* FIXME: Just send an expose event, lock the event queue */
	pthread_mutex_lock (&draw_mutex);
	widget_draw (&elapsed.widget);
	XFlush (elapsed.widget.display);
	pthread_mutex_unlock (&draw_mutex);
}

void
playingUriChanged (proxy, uri, data)
	DBusGProxy *proxy;
	char *uri;
	void *data;
{
	struct id3_file *file;
	char *track_str;

	file = id3_file_open (file_uri (uri), ID3_FILE_MODE_READONLY);

	if (title_text)
		free (title_text);
	if (artist_text)
		free (artist_text);

	title.buffer = title_text = get_id3str (file, ID3_FRAME_TITLE);
	artist.buffer = artist_text = get_id3str (file, ID3_FRAME_ARTIST);

	track_str = get_id3str (file, ID3_FRAME_TRACK);
	sprintf (track_text, "%02i", atoi (track_str));
	free (track_str);
	track.buffer = track_text;

	/* FIXME: Just send an expose event, lock the event queue */
	pthread_mutex_lock (&draw_mutex);
	widget_draw (&title.widget);
	widget_draw (&artist.widget);
	widget_draw (&track.widget);
	XFlush (elapsed.widget.display);
	pthread_mutex_unlock (&draw_mutex);

	id3_file_close (file);
}

/* UI Button callbacks */

int
previous_track (widget)
	struct widget *widget;
{
	GError *error;

	if (!proxy)
		return 0;
	error = NULL;
	dbus_g_proxy_call (proxy, "previous", &error,
		G_TYPE_INVALID, G_TYPE_INVALID);

	return (error == NULL);
}

int
play_pause (widget)
	struct widget *widget;
{
	GError *error;

	if (!proxy)
		return 0;
	error = NULL;
	dbus_g_proxy_call (proxy, "playPause", &error,
		G_TYPE_BOOLEAN, 0, G_TYPE_INVALID, G_TYPE_INVALID);

	return (error == NULL);
}

int
next_track (widget)
	struct widget *widget;
{
	GError *error;

	if (!proxy)
		return 0;
	error = NULL;
	dbus_g_proxy_call (proxy, "next", &error,
		G_TYPE_INVALID, G_TYPE_INVALID);

	return (error == NULL);
}

/* The player backend thread loop */

void *
player (data)
	void *data;
{
	GError *error;
	DBusGConnection *conn;
	char *uri;
	guint32 elapsed;

	g_type_init ();

	error = NULL;
	conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	proxy = dbus_g_proxy_new_for_name (conn, "org.gnome.Rhythmbox",
		"/org/gnome/Rhythmbox/Player", "org.gnome.Rhythmbox.Player");

	/* Initial settings */
	dbus_g_proxy_call (proxy, "getElapsed", &error, G_TYPE_INVALID,
		G_TYPE_UINT, &elapsed, G_TYPE_INVALID);
	elapsedChanged (proxy, elapsed, NULL);

	dbus_g_proxy_call (proxy, "getPlayingUri", &error, G_TYPE_INVALID,
		G_TYPE_STRING, &uri, G_TYPE_INVALID);
	playingUriChanged (proxy, uri, NULL);
	g_free (uri);

	/* Watch for changes */
	dbus_g_proxy_add_signal (proxy, "elapsedChanged",
		G_TYPE_UINT, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (proxy, "elapsedChanged",
		G_CALLBACK(elapsedChanged), NULL, NULL);

	dbus_g_proxy_add_signal (proxy, "playingUriChanged",
		G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (proxy, "playingUriChanged",
		G_CALLBACK(playingUriChanged), NULL, NULL);

	/* Start the ball rolling */
	g_main_loop_run (g_main_loop_new (NULL, 1));

	return 0;
}
