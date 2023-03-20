# these are flags or arguments that'll be passed to the C compiler
# -Wall is for almost all warning the cc compiler can give you
# -g is for debugging
#CFLAGS = -O0 -g
#LDFLAGS = -Wall

# use $< infront of the last path with gcc to send the arguments to the back (?)

CFLAGS += `pkg-config --cflags libdrm`
LDFLAGS += `pkg-config --libs libdrm`
LDLIBS += `pkg-config --libs libdrm`
PULSELIBS += `pkg-config --cflags --libs libpulse-simple`
CAIROLIBS += `pkg-config --cflags --libs cairo` -lts
AMR = src/matn.c src/alwaan.c src/utf8.c src/array.c src/helper.c src/msfoof.c src/amr.c
MLF = src/tarmeez.c src/malaf.c
KTB = src/canvas.c src/tarmeez.c src/eqonah.c src/xat.c src/malaf.c src/kaatib.c
CMPSR = src/composer.c
CAIJPGFLAGS = `pkg-config libjpeg --cflags --libs`
FTFLAGS += `pkg-config --cflags freetype2` -lfreetype
FLAGS = -Iinclude -Llib -lrt -lm
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

s3l-test:
	gcc -g -o bin/s3l-test $(AMR) $(KTB) src/s3l-test.c $(FLAGS)

pulse:
	gcc -o bin/pulse $(AMR) $(KTB) src/pulse.c $(FLAGS) $(PULSELIBS)

sox2:
	gcc -o bin/sox2 src/sox2.c -lsox -lm

svg:
	gcc -Wall -g -O2 -o bin/svg src/svg.c `pkg-config --cflags --libs librsvg-2.0`

soundio:
	gcc -Wall -g -O2 -o bin/soundio src/soundio.c -lm -lsoundio

app.dukjs:
	gcc -g -o bin/app.dukjs src/sdl.c `pkg-config --libs --cflags sdl2` -lGL -lm -lGLEW \
	$(AMR) $(KTB) $(CMPSR) src/app.dukjs.c -lduktape $(CAIROLIBS) $(FLAGS)

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
	gcc -o bin/drm src/drm.c $< $(CFLAGS) $(LDFLAGS) $(LDLIBS)

madxal:
	rm -f bin/madxal
	gcc -o bin/madxal $(AMR) src/malaf.c src/masaadir.c \
	src/madxal.c $(FLAGS)

masaadir:
	rm -f bin/masaadir
	gcc -o bin/masaadir src/masaadir.c

array-test:
	rm -f bin/array-test
	gcc -o bin/array-test src/array-test.c

inotify:
	rm -f bin/inotify
	gcc -o bin/inotify src/inotify.c

x11:
	rm -f bin/x11
	gcc -pthread -o bin/x11 $(AMR) src/malaf.c src/tarmeez.c src/eqonah.c \
	src/xat.c src/x11.c \
	-lX11 -lrt -lm -lXext $(FTFLAGS)

xtest:
	rm -f bin/xtest
	gcc -pthread -o bin/xtest $(AMR) src/malaf.c src/tarmeez.c src/eqonah.c \
	src/xat.c src/nabad.c src/xtest.c \
	-lX11 -lrt -lm -lXext

xat-hur:
	clear
	rm -f bin/xat
	gcc -o bin/xat src/alwaan.c src/helper.c src/matn.c \
	src/msfoof.c src/malaf.c src/utf8.c src/tarmeez.c src/eqonah.c src/xat.c \
	src/xat-hur.c $(FLAGS)

msfoof-test:
	clear
	gcc -g -o bin/msfoof-test $(AMR) src/msfoof-test.c $(FLAGS)

cairo-fb:
	gcc -o bin/cairo-fb src/cairo-fb.c $(FLAGS) $(CAIROLIBS)

cairo-test:
	gcc lib/cairo_jpg.o \
		-o bin/cairo-test $(AMR) $(KTB) src/cairo-test.c $(CAIROLIBS) $(FLAGS) $(CAIJPGFLAGS)
