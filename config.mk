# xmenu version
VERSION = 0.1

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# includes and libs
INCS =
LIBS =

# compiler flags
CPPFLAGS = -DVERSION=\"$(VERSION)\" # dynamically create global macro via c preprocessor
CFLAGS   = -std=c23 -pedantic -Wall -Os $(INCS) $(CPPFLAGS)
LDFLAGS  = $(LIBS)

# uncomment for debugging (overrides optimization)
#CFLAGS   = -std=c23 -pedantic -Wall -g -O0 $(INCS) $(CPPFLAGS)

# compiler and linker
CC = gcc
