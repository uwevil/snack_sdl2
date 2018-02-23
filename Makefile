BUILDDIR=build
PROG=snack
# SRC := snack
# CXX := gcc
# CFLAGS := -framework SDL2

all: build clean

.PHONY: build
build:
	# gcc -o $(BUILDDIR)/snack ./snack/main.c `sdl2-config --cflags --libs`
	gcc -framework SDL2 -o $(BUILDDIR)/$(PROG) ./$(PROG)/main.c

.PHONY: run
run:
	$(BUILDDIR)/$(PROG)

.PHONY: $(PROG)
$(PROG):
	gcc -framework SDL2 -o $(BUILDDIR)/$(PROG) ./$(PROG)/main.c
	$(BUILDDIR)/$(PROG)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*
