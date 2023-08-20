program := minesweeper-wfc

CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
LDFLAGS := $$(pkg-config allegro-5 allegro_image-5 --libs --cflags)

ifneq ($(D),1)
CFLAGS += -O3
else
CFLAGS += -Og -g -DDEBUG
endif

all: $(program)

objs := main.o render.o queue.o wave_function.o

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

minesweeper-wfc: $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(program) $(objs) $(deps)
