program = pixedit
sources := $(wildcard *.c)
hdrs := $(wildcard *.h)
objects = $(sources:.c=.o)
CFLAGS = `pkg-config --cflags sdl2 SDL2_image` -Wall -g -std=c99
CFLAGS += -I ~/include -L ~/lib -Wl,-rpath=$(HOME)/lib
CFLAGS += -Werror -Wfatal-errors
CFLAGS += -fPIC
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lgcu

ifeq ($(OPTIMIZE), true)
	CFLAGS += -O3
endif

all: $(program)

$(program): $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(program) $(objects)
