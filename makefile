program := minesweeper-wfc

CFLAGS := -Wall -Wextra -Werror -MMD -I./allegro5/include -I./allegro5/addons/image -I./allegro5-build/include
LDFLAGS := -L./allegro5-build/lib -lallegro_monolith-static -lm -lSM -lICE -lX11 -lXext -lXcursor -lXi -lXinerama -lXrandr -lXss -lOpenGL -lGLU -lGLX -lpng -lz

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

objs := main.o render.o queue.o wave_function.o board.o

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

minesweeper-wfc: $(objs) allegro5-build/lib/liballegro_monolith-static.a
	$(CC) -o $@ $(objs) $(LDFLAGS)

%.o: %.c allegro5-build/lib/liballegro_monolith-static.a
	$(CC) $(CFLAGS) -o $@ -c $<

allegro5-build/lib/liballegro_monolith-static.a:
	mkdir -p allegro5-build
	cmake -S allegro5/ -B allegro5-build/ -DALLEGRO_TARGETS=allegro_monolith -DSHARED=off -DWANT_AUDIO=off -DWANT_COLOR=off -DWANT_DEMO=off -DWANT_DOCS=off -DWANT_DOCS_HTML=off -DWANT_DOCS_MAN=off -DWANT_EXAMPLES=off -DWANT_FONT=off -DWANT_IMAGE=on -DWANT_IMAGE_FREEIMAGE=off -DWANT_IMAGE_JPG=off -DWANT_IMAGE_WEBP=off -DWANT_MEMFILE=off -DWANT_MONOLITH=on -DWANT_NATIVE_DIALOG=off -DWANT_PHYSFS=off -DWANT_POPUP_EXAMPLES=off -DWANT_PRIMITIVES=off -DWANT_RELEASE_LOGGING=off -DWANT_TESTS=off -DWANT_TTF=off -DWANT_VIDEO=off
	$(MAKE) -j`nproc` -C allegro5-build/

minesweeper-wfc-Linux.tar.gz: minesweeper-wfc
	tar -czf minesweeper-wfc-Linux.tar.gz minesweeper-wfc minesweeper-wfc.png

clean:
	rm -rf $(program) $(program).exe $(objs) $(deps)
