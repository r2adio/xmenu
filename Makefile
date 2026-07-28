include config.mk

BIN = bin

CORE_SRC = libxmenu.c
CLI_SRC  = xmenu_cli.c

CORE_OBJ = $(BIN)/$(CORE_SRC:.c=.o)
CLI_OBJ  = $(BIN)/$(CLI_SRC:.c=.o)

LIB = $(BIN)/libxmenu.a

all: xmenu-cli

$(BIN)/%.o: %.c
	@mkdir -p $(BIN)
	$(CC) -c $(CFLAGS) -o $@ $<

$(CORE_OBJ) $(CLI_OBJ): xmenu.h config.mk

$(LIB): $(CORE_OBJ)
	$(AR) rcs $@ $^

xmenu-cli: $(BIN)/cli/xmenu

$(BIN)/cli/xmenu: $(CLI_OBJ) $(LIB)
	@mkdir -p $(BIN)/cli
	$(CC) -o $@ $(CLI_OBJ) $(LIB) $(LDFLAGS)

xmenu-mac:
	@echo "xmenu-mac: not implemented"

xmenu-linux:
	@echo "xmenu-linux: not implemented"

clean:
	rm -r $(BIN) xmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p xmenu-$(VERSION)
	cp LICENSE Makefile config.mk xmenu.h $(CORE_SRC) $(CLI_SRC) termbox2.h xmenu-$(VERSION)
	tar -cf xmenu-$(VERSION).tar xmenu-$(VERSION)
	gzip xmenu-$(VERSION).tar
	rm -rf xmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN)/cli/xmenu $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xmenu

.PHONY: all clean dist install uninstall xmenu-cli xmenu-mac xmenu-linux
