# xmenu version
VERSION = 0.1

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# includes and libs
INCS =
LIBS =

# flags
CFLAGS   = -std=c23 -pedantic -Wall -Os $(INCS)
LDFLAGS  = $(LIBS)

# compiler and linker
CC = gcc
