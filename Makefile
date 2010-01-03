CFLAGS += -Wall
LDFLAGS = -lXpm -lX11 -lXext

all: pew
pew: pew.o ui.o widget.o text.o button.o frame.o

pew.o: controls/*.xpm font/*.xpm
pew.o text.o: text.h
pew.o button.o: button.h
pew.o frame.o: frame.h
ui.h widget.o text.o button.o frame.o: widget.h

clean:
	rm -f pew *.o
