CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS := $(shell pkg-config --libs gtk+-3.0)
CC = gcc

SRCDIR=src
OUTDIR=out
LDDIR=$(SRCDIR)/lib

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))
LDLIBS=$(patsubst $(LDDIR)%.c, $(OUTDIR)/%.so, $(SRCS))

INCLUDE=-Iinclude

$(OUTDIR)/%.o : $(SRCDIR)/%.c Makefile
	$(CC) -c $< $(INCLUDE) $(CFLAGS) -o $@ $(LDFLAGS)

PROJ_NAME=codEditor

all: $(OBJS)
	$(CC) $^ $(CFLAGS) -o $(PROJ_NAME) $(LDFLAGS)

clean:
	rm out/* && rm codEditor

