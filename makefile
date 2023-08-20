program := minesweeper-wfc

CFLAGS := -Wall -Wextra -Werror -MMD
LDFLAGS := $$(pkg-config allegro-5 allegro_image-5 --libs --cflags)

ifneq ($(W),1)
CC := gcc
else
CC := x86_64-w64-mingw32-gcc
endif

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
	rm -f $(program) $(program).exe $(objs) $(deps)
