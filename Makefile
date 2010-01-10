# Common
CFLAGS += -Wall -g -O0

# Ui
LDFLAGS = -lXpm -lX11 -lXext

# Player backend
DEPS = glib-2.0 dbus-glib-1 id3tag
CFLAGS += -Wall $(shell pkg-config --cflags $(DEPS))
LDFLAGS += $(shell pkg-config --libs $(DEPS))

all: pew
pew: pew.o ui.o widget.o text.o button.o frame.o player.o

pew.o: controls/*.xpm font/*.xpm
pew.o text.o: text.h
pew.o button.o: button.h
pew.o frame.o: frame.h
pew.o ui.o: ui.h
ui.o widget.o text.o button.o frame.o: widget.h

clean:
	rm -f pew *.o
