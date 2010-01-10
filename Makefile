# Common
CFLAGS += -Wall -g -O0

# Ui
LDFLAGS = -lXpm -lX11 -lXext

# Player backend
DEPS = glib-2.0 dbus-glib-1 id3tag
CFLAGS += -Wall $(shell pkg-config --cflags $(DEPS))
LDFLAGS += $(shell pkg-config --libs $(DEPS))

all: wmplayer
wmplayer: wmplayer.o ui.o widget.o text.o button.o frame.o player.o

wmplayer.o: controls/*.xpm font/*.xpm
wmplayer.o text.o: text.h
wmplayer.o button.o: button.h
wmplayer.o frame.o: frame.h
wmplayer.o ui.o: ui.h
ui.o widget.o text.o button.o frame.o: widget.h

clean:
	rm -f wmplayer *.o
