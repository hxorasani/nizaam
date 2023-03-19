/* pulse: 
 * TODO:
 * 	give amr a hint that we want musheer and raees before connection
 * 	then amr should handle all the actual logic and errors
 * */

#include "pulse.h"

#define XATAA 0

int started = 0, elapsed = 0, playing = 0, bufSize = 64, period = 10, error;
float freq = 900, factor = 4, volume = 100;
struct timespec waqt;
double *avg;
pa_simple *s;
pa_sample_spec ss;
kaatib ktb;

int8_t userFunc(double time) {
    double op = sin(factor * 3.14 * freq * time);
    return op * volume;
}
void _mainLoop() {
	char buff[2*bufSize];
	float pct = 0;
	char firstgo = 1;
	while (playing) {
		for ( int i = 0; i < bufSize; i++ ) {
			pct = (elapsed/100000.0) / 2;
			int8_t word = userFunc(elapsed/1000000.0) * (1-pct);
			elapsed += period;
			buff[i*2+0] = (word >> 0) & 0xff;
			buff[i*2+1] = (word >> 1) & 0xff;
			if (firstgo) {
				avg[i] = ((buff[i*2+0] + buff[i*2+1]) / 2.0)/128.0;
				if (i == bufSize-1) firstgo = 0;
			}
		}
		if (pct >= 1) playing = 0, elapsed = 0;
		pa_simple_write(s, buff, 2*bufSize, &error);
	}
}
void drawraees() {
	msfoof *r = &WJHH.raees.mtrx;
	int w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h, c1 = 0xff00ff, c2 = 0xffffff,
		yh = h/2, cell = w/bufSize;

	msfoof_clear(r, 0);

	for ( int i = 0; i < bufSize; ++i ) {
		int x = i*cell, y = yh, ch = abs(avg[i]*yh);
		if (avg[i] < 0) y = y - ch;
//		msfoof_rect(r, x, y, cell, avg[i], c1, -1);
		msfoof_rect(r, x, y, cell, ch, -1, c2);
//		msfoof_line(r, x, y, x, y+ch, c2);
	}
	
	kaatib_draw(&ktb);

	started = 1;
}
void changecolor() {
	msfoof *m = &WJHH.mshr.mtrx;
	msfoof_clear(m, 0);
	int w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h, c1 = 0xffffff, c2 = 0xffffff;
	matrix_circle(m, w/2, h/2, w/3, 0, 360, c1, c2);
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			mafateeh m = { 0 }; char yes = 0;
			str2mafateeh(w->qadrstr, &m);
			if (m.state && !playing) {
				if (m.key == KEY_1) freq = 100, yes = 1;
				if (m.key == KEY_2) freq = 200, yes = 1;
				if (m.key == KEY_3) freq = 300, yes = 1;
				if (m.key == KEY_4) freq = 400, yes = 1;
				if (m.key == KEY_5) freq = 500, yes = 1;
				if (m.key == KEY_6) freq = 600, yes = 1;
				if (m.key == KEY_7) freq = 700, yes = 1;
				if (m.key == KEY_8) freq = 800, yes = 1;
				if (m.key == KEY_9) freq = 900, yes = 1;
				if (m.key == KEY_UP) volume += 10, yes = 2;
				if (m.key == KEY_DOWN) volume -= 10, yes = 2;
				if (m.key == KEY_RIGHT) factor += .5, yes = 2;
				if (m.key == KEY_LEFT) factor -= .5, yes = 2;
				if (yes) {
					if (factor < 0) factor = 0.5;
					if (volume < 10) volume = 10;
					if (volume > 100) volume = 100;
					if (yes == 1) {
						playing = !playing;
						_mainLoop();
					}
					unsigned char *tmp = helper_stringify("freq: %.1fHz\nfactor: %.1f\nvol: %.0f\0", freq, factor, volume);
					kaatib_baddal(&ktb, tmp);
					free(tmp);
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
		if (w->ism == MUSHEER) {
			if (!started)
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				changecolor();
				amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				if (w->qadr == 1 && ktb.mst3d == 0) {
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = ktb.x.h*ktb.g.s;
					ktb.mtrx = WJHH.raees.mtrx;
					kaatib_xataa(&ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
			}
			if (!started)
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					WJHH.raees.mutadarrar = 0;
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	tarmeez_core();
	
	avg = (double *) malloc(sizeof(double) * bufSize);
	memset(avg, 0, sizeof(double) * bufSize );

	kaatib_init(&ktb); // will do matn -> huroof
	ktb.hrfmsfh = 4;
	ktb.g.s = 2;

	ss.format = PA_SAMPLE_S16LE;
	ss.channels = 1;
	ss.rate = 44100;
	
	s = pa_simple_new(
		NULL,				// Use the default server.
		"Synth",			// Our application's name.
		PA_STREAM_PLAYBACK,
		NULL,				// Use the default device.
		"Music",			// Description of our stream.
		&ss,				// Our sample format.
		NULL,				// Use default channel map
		NULL,				// Use default buffering attributes.
		NULL				// Ignore error code.
	);
	
	amr_towheed("pulse");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
