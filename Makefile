include config.mk

SRC = xmenu.c
OBJ = $(SRC:.c=.o)

all: xmenu

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): config.mk

xmenu: xmenu.o
	$(CC) -o $@ xmenu.o $(LDFLAGS)

clean:
	rm -f xmenu $(OBJ) xmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p xmenu-$(VERSION)
	cp LICENSE Makefile config.mk $(SRC) termbox2.h xmenu-$(VERSION)
	tar -cf xmenu-$(VERSION).tar xmenu-$(VERSION)
	gzip xmenu-$(VERSION).tar
	rm -rf xmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xmenu $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xmenu

.PHONY: all clean dist install uninstall
