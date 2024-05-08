# -Wall is for almost all warning the cc compiler can give you
# -g is for debugging
# -c means don't run the linker
# use $< infront of the last path with gcc to send the arguments to the back (?)

CC=gcc

AMR = src/map.c src/matn.c src/alwaan.c src/utf8.c \
	  src/array.c src/helper.c src/msfoof.c src/amr.c src/ttf2mesh.c \
	  src/common.c src/shader.c
AUD = src/miniaudio.c
MLF = src/tarmeez.c src/malaf.c
KTB = src/canvas.c src/tarmeez.c src/eqonah.c src/xat.c src/malaf.c src/kaatib.c
CMPSR = src/composer.c
FTFLAGS += `pkg-config --cflags freetype2` -lfreetype
FLAGS = -lrt -lm
INCLIB = -Iinclude -Llib

all: xidmaat

xidmaat:
	rm -f bin/xidmaat
	gcc -o bin/xidmaat $(AMR) $(MLF) src/xidmaat.c $(FLAGS)

waajihah:
	gcc -o bin/waajihah $(AMR) src/waajihah.c $(FLAGS)

maxraj:
	gcc -o bin/maxraj $(AMR) src/maxraj.c $(FLAGS)

freetype:
	gcc -g -o bin/freetype $(AMR) $(KTB) src/freetype.c $(FLAGS)

svg:
	gcc -Wall -g -O2 -o bin/svg src/svg.c `pkg-config --cflags --libs librsvg-2.0`

soundio:
	gcc -Wall -g -O2 -o bin/soundio src/soundio.c -lm -lsoundio

nizaam:
#	feed the linker a comma-sep list of include dirs
	gcc -g -pthread -o bin/nizaam src/app.dukjs.c src/sgl.c src/sdl.c src/nuk.c \
	src/ttf.c src/aud.c src/asio.c src/codec.c \
	`pkg-config --libs --cflags sdl2` -lGL -lm -lGLEW -lGLU \
	-Wl,-rpath=$(CURDIR)/lib \
	$(INCLIB) $(MLF) $(CMPSR) -lduv -ldschema -luv -lamr -laud -lduktape $(FLAGS)

aud.so: $(AUD:.c=.o)
	gcc -shared -o lib/libaud.so build/*.o
	rm -rf build/*.o

amr.so: $(AMR:.c=.o)
	gcc -shared -o lib/libamr.so build/*.o
	rm -rf build/*.o

src/%.o: src/%.c
	$(CC) -fPIC -c -o build/$*.o $<

duktape-install:
	cd src/duktape-2.7.0
	make -f Makefile.sharedlibrary
	make INSTALL_PREFIX=../nizaam/lib -f Makefile.sharedlibrary install

mudeer:
	gcc -g -o bin/mudeer $(AMR) src/tarmeez.c src/eqonah.c src/kaatib.c \
	src/xat.c src/malaf.c src/mudeer.c $(FLAGS) $(FTFLAGS)

glfw:
	gcc -o bin/glfw src/glfw.c $(INCLIB) -lglfw -lGL -lm -lGLEW

sdl:
	gcc -o bin/sdl $(AMR) $(KTB) $(CMPSR) \
	src/sdl.c $(INCLIB) `pkg-config --libs --cflags sdl2` -lGL -lm -lGLEW \
	-lduktape $(CAIROLIBS) $(FLAGS)

weston-client:
	cc -o bin/weston-client src/weston-client.c -lwayland-client

weston-client-egl:
	gcc -o bin/weston-client-egl src/weston-client-egl.c -lwayland-client \
	-lwayland-egl -lgbm -lEGL -lGL
	./bin/weston-client-egl

drm:
	rm -f bin/drm
	gcc -o bin/drm src/drm.c $< `pkg-config --libs --flags libdrm`

madxal:
	rm -f bin/madxal
	gcc -o bin/madxal $(AMR) src/malaf.c src/masaadir.c \
	src/madxal.c $(FLAGS)

test:
	gcc -o bin/test src/test.c

threads:
	gcc -fno-strict-aliasing -g -pthread -o bin/threads src/threads.c $(INCLIB) -luv

masaadir:
	rm -f bin/masaadir
	gcc -o bin/masaadir src/masaadir.c

x11:
	rm -f bin/x11
	gcc -pthread -o bin/x11 $(AMR) src/malaf.c src/tarmeez.c src/eqonah.c \
	src/xat.c src/x11.c \
	-lX11 -lrt -lm -lXext $(FTFLAGS)
