// instead of unsigned int, uint32_t is portable for accessing the pixel data

#include "msfoof.h"

#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)
#define NMVJX 2
#define NMVJY 2
	
float todegrees(float angle) {
	return angle * (180 / M_PI);
}
float toradians(float angle) {
	return angle * (M_PI / 180);
}
int isinside(int x, int y, ihaatah *g) {
	if (x > g->x && y > g->y && x < g->x+g->w && y < g->y+g->h) return 1;
	return 0;
}
int mushtamil(int x, int y, ihaatah *g) { // contains, includes
	if (x >= g->x && y >= g->y && x <= g->x+g->w && y <= g->y+g->h) return 1;
	return 0;
}
int mushtamilf(float x, float y, ihaatahf *g) { // contains, includes
	if (x >= g->x && y >= g->y && x <= g->x+g->w && y <= g->y+g->h) return 1;
	return 0;
}
int isinsidef(float x, float y, ihaatahf *g) {
	if (x > g->x && y > g->y && x < g->x+g->w && y < g->y+g->h) return 1;
	return 0;
}
int isnuqtahinside(nuqtah *n, ihaatah *g) {
	return isinside(n->x, n->y, g);
}
char do_rects_overlap(ihaatah *a, ihaatah *b) { // is rect overlapping
	return (
			a->x <= (b->x+b->w) &&
			b->x <= (a->x+a->w) &&
			a->y <= (b->y+b->h) &&
			b->y <= (a->y+a->h)
			);
}
void ihaatah_ws3(ihaatah *ihth, int x, int y, int x2, int y2) {
	if (ihth) {
//		printf("ihaatah: %d %d\n", ihth->w, ihth->h);
//		printf("kaatib mutadarrar: %d %d %d %d\n", x, y, x2, y2);
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		if (ihth->w == 0) ihth->x = x;
		else if (x < ihth->x && x > -1) ihth->x = x;

		if (ihth->h == 0) ihth->y = y;
		else if (y < ihth->y && y > -1) ihth->y = y;

		if (x+x2 > ihth->w) ihth->w = x+x2;
		if (y+y2 > ihth->h) ihth->h = y+y2;
	}
}
void ihaatah_nazaf(ihaatah *ihth) {
	if (ihth) { ihth->x = 0; ihth->y = 0; ihth->w = 0; ihth->h = 0; }
}
void ihaatahf_nazaf(ihaatahf *ihth) {
	if (ihth) { ihth->x = 0; ihth->y = 0; ihth->w = 0; ihth->h = 0; }
}
char ihaatah_yajid(ihaatah *ihth) { // _found
	if (ihth->w && ihth->h) { return 1; }
	return 0;
}
char ihaatahf_yajid(ihaatahf *ihth) { // _found
	if (ihth->w && ihth->h) { return 1; }
	return 0;
}
void ihaatah_nasax(ihaatah *h, ihaatah *m) {
	h->x = m->x; h->w = m->w; h->s = m->s;
	h->y = m->y; h->h = m->h; h->i = m->i;
}
void ihaatahf_nasax(ihaatahf *h, ihaatahf *m) {
	h->x = m->x; h->w = m->w; h->s = m->s;
	h->y = m->y; h->h = m->h; h->i = m->i;
}
void ihaatah2f_nasax(ihaatahf *h, ihaatah *m) {
	h->x = m->x; h->w = m->w; h->s = m->s;
	h->y = m->y; h->h = m->h; h->i = m->i;
}
void ihaatahf2_nasax(ihaatah *h, ihaatahf *m) {
	h->x = m->x; h->w = m->w; h->s = m->s;
	h->y = m->y; h->h = m->h; h->i = m->i;
}
u_char * ihaatah2str(ihaatah m) {
	return helper_stringify("%d%c%d%c%d%c%d", m.x, 130, m.y, 130, m.w, 130, m.h);
}
void str2ihaatah(unsigned char *barzax, ihaatah *ihth) {
	int strlimit = 256;
	unsigned char str[strlimit];
	int coli = 0, col = 0; // key column index
	int stri = 0; // key string index
	long key = 0, length = strlen(barzax); // key string as long
	unsigned char c;
	
	for (long i = 0; i < length+1; ++i) {
		if (i == length) c = 130; // to allow indexing the last column
		else c = barzax[i];

		if (c == 130 || i == length) { // came across a sep or new line
			// either way
			str[stri] = '\0'; // null terminate string
			if (col < 6) {
				key = strtol(str, (char **) &str[stri], 10);
				if (col == 0) ihth->x	= key;
				if (col == 1) ihth->y	= key;
				if (col == 2) ihth->w	= key;
				if (col == 3) ihth->h	= key;
			}

			matn_nazaf_str(str, strlimit); // reset string
			stri = 0; // reset key string index
			coli = 0; // reset key column index

			if (c == 130) { // only when it's a new sep
				col++; // going to the next column
			}
		} else {
			if (stri < strlimit) {
				str[stri] = c;
				stri++;
			}
			coli++;
		}
	}
}
float coordtoangle(float x0, float y0, float x1, float y1) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float ang = todegrees( atan2f(dy, dx) );
	return (ang < 0 ? ang + 360 : ang);
}
float coordtoanglerad(float x0, float y0, float x1, float y1) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float ang = dy / dx;
	if (dx == 0) ang = 1;
	return ang;
}
void angletocoord(float x, float y, float r, float a, nuqtahf *p) { // degrees
	float hpi = M_PI / 180;
//	float na = a - 180; // shift angle -180
//	p->x = x + r * cosf( na * hpi );
//	p->y = y + r * sinf( na * hpi );
	p->x = x + r * cosf( a * hpi );
	p->y = y + r * sinf( a * hpi );
}
int distance(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1; dx *= dx;
	int dy = y2 - y1; dy *= dy;
	return sqrt( dx + dy );
}
float distancef(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1; dx *= dx;
	float dy = y2 - y1; dy *= dy;
	return sqrtf( dx + dy );
}
int inpolygon(int x, int y, nuqtah poly[], int length) {
	int isinside = 0;
	if (length) {
		int minX = poly[0].x, maxX = poly[0].x;
		int minY = poly[0].y, maxY = poly[0].y;
		for (int n = 1; n < length; n++) {
			minX = fmin(poly[n].x, minX);
			maxX = fmax(poly[n].x, maxX);
			minY = fmin(poly[n].y, minY);
			maxY = fmax(poly[n].y, maxY);
		}

		if (x < minX || x > maxX || y < minY || y > maxY) {
			return 0;
		}

		int i = 0, j = length - 1;
		for (i, j; i < length; j = i++) {
			if ( (poly[i].y > y) != (poly[j].y > y) &&
					x < (poly[j].x - poly[i].x)
						* (y - poly[i].y)
						/ (poly[j].y - poly[i].y) + poly[i].x )
			{
				isinside = !isinside;
			}
		}
	}
	return isinside;
}
int bordertype(int r, int f, int j, int k, int x, int y) {
	int d = distance(j, k, x, y);
	if (f) return d < r;
	else return d >= (r) && d <= r;
}
float bordertypef(float r, int f, float j, float k, float x, float y) {
	float d = distancef(j, k, x, y);
	if (f) return d < r;
	else return d >= (r) && d <= r;
}

u_char msfoof_get_alif(msfoof *m, int x, int y) {
	if (x >= 0 && y >= 0 && x < m->wa && y < m->ha)
		return (u_char) *(m->alif + y * m->wa + x);
	else
		return 0;
}
int msfoof_get(msfoof *m, int x, int y) {
	if (x >= 0 && y >= 0 && x < m->w && y < m->h)
		return (int) *(m->barzax + y * m->w + x);
	else
		return -1;
}
int msfoof_get_avg(msfoof *m, int x, int y) {
	if (x >= 0 && y >= 0 && x < m->w && y < m->h) {
		int num, r = 0, g = 0, b = 0, d = 1;
		for (int i = x - d; i < x + d; i++) {
			for (int j = y - d; j < y + d; j++) {
				int c = msfoof_get(m, i, j);

				/* If the pixel is outside the canvas, skip it */
				if (c == -1) continue;

				/* If the pixel is outside the circle, skip it */
				if (distance(x, y, i, j) > d) continue;

				lwn l;
				rgb2lwn(c, &l);
				r += l.r*l.r*l.r;
				g += l.g*l.g*l.g;
				b += l.b*l.b*l.b;
				num++;
			}
		}
		lwn l = {0, cbrt(r/num), cbrt(g/num), cbrt(b/num)};
		return lwn2rgb(&l);
	} else
		return -1;
}
void msfoof_get_owst_alif(msfoof *m, int x, int y, lwnf *lxrj) {
	x = x*m->nmvj.x; y = y*m->nmvj.y;
	if (x >= 0 && y >= 0 && x < m->wa && y < m->ha) {
		lwn l;
		int r = 0, g = 0, b = 0, d = 1, color, num = m->nmvj.x*m->nmvj.y;
		float alpha = 0.0;
		for (int i = x; i < x + m->nmvj.x; i++) {
			for (int j = y; j < y + m->nmvj.y; j++) {
				unsigned char c = msfoof_get_alif(m, i, j);
				
				if (c) {
					alpha++;

//					rgb2lwn(char2rgb(c), &l);
//					r += l.r;
//					g += l.g;
//					b += l.b;
				}

//				num++;
			}
		}
//		if (alpha>9)
//		printf("%f\n", alpha);
		if (alpha) alpha /= (m->nmvj.x*m->nmvj.y);
//		*lxrj = (lwnf) {alpha, cbrt(r/num/255.0), cbrt(g/num/255.0), cbrt(b/num/255.0)};
//		*lxrj = (lwnf) {alpha, sqrt(r/num), sqrt(g/num), sqrt(b/num)};
//		*lxrj = (lwnf) {alpha, (r/num)/255.0, (g/num)/255.0, (b/num)/255.0};
		*lxrj = (lwnf) {alpha, 0, 0, 0};
	}
}

void msfoof_set_alif(msfoof *m, int x, int y, u_char color) {
	if (x >= 0 && y >= 0 && x < m->wa && y < m->ha)
		*(m->alif + y * m->wa + x) = color;
}
void msfoof_set(msfoof *m, int x, int y, u_int color) {
//	if (m->smooth) {
////		msfoof_rect(m, x, y, 1, 1, -1, color);
//	} else
	if (x >= 0 && y >= 0 && x < m->w && y < m->h)
		*(m->barzax + y * m->w + x) = color;
}
void msfoof_set_avg(msfoof *m, int x, int y, u_int color) {
	if (x >= 0 && y >= 0 && x < m->w && y < m->h) {
		int r = 1, tmp;
		for (int j = x-r; j <= x+r; j++)
			for (int k = y-r; k <= y+r; k++) {
				if (bordertype(r, 1, j, k, x, y)) {
//					msfoof_set(m, j, k, color);
					tmp = msfoof_get_avg(m, j, k);
//					if (tmp == -1) {}
//					else
					if (tmp == 0)
						msfoof_set( m, j, k, color );
					else
						msfoof_set( m, j, k, alwaan_avg_pct(tmp, color, .1) );
				}
			}
	}
}
void msfoof_set_blend(msfoof *m, int x, int y, u_int color, float br) {
	if (x >= 0 && y >= 0 && x < m->w && y < m->h) {
		if (br < 0.0f) br = 0.0f;
		if (br > 1.0f) br = 1.0f;
		if (br < 1.0f) {
//			color = alwaan_add( alwaan_pct(color, 1-br), msfoof_get( m, x, y ) );
			int oldcolor = msfoof_get( m, x, y );
			color = alwaan_add(
				alwaan_pct( color, br ),
				alwaan_pct( oldcolor, 1.0f-br )
			);
			msfoof_set( m, x, y, color );
		}
	}
}

void msfoof_composite(msfoof *s, msfoof *t, ihaatah *g) {
	// copy the entire s msfoof onto t starting at g.x & g.y
	unsigned int x = 0, y = 0, w = s->w, h = s->h;
	if (g != NULL) {
		x = g->x	;	y = g->y	;
		w = g->w	;	h = g->h	;
	}

//	printf("msfoof_composite: %d %d, %d %d\n", x, y, w, h);

	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {
			msfoof_set( t, i+x, j+y, msfoof_get(s, i, j) );
		}
	}
}
void msfoof_composite_clipped(msfoof *s, msfoof *t, ihaatah *si, ihaatah *ti) {
	// copy the entire s msfoof onto t starting at g.x & g.y
	int x = 0, y = 0, w = s->w, h = s->h, c;
	if (si != NULL) {
		x = si->x	;	y = si->y	;
		w = si->w	;	h = si->h	;
	}

//	printf("msfoof_composite: %d %d, %d %d\n", x, y, w, h);

	// si ihaatah is where to start and end in the s msfoof
	// ti ihaatah is where to start in the t msfoof

	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {
			c = msfoof_get(s, x+i, y+j);
			if (c != -1) msfoof_set( t, ti->x+i, ti->y+j, c );
		}
	}
}
void msfoof_scale(msfoof *s, msfoof *t, ihaatah *si, nuqtahf *ti) {
	// if no t msfoof, than assume self copy
	if (t == NULL) t = s;
	
	// target cx,cy is actually ratios
	float wr = ti->cx, hr = ti->cy;
	
	// copy the entire s msfoof onto t starting at g.x & g.y
	int x = 0, y = 0, w = 0, h = 0, c;
	if (si != NULL) {
		x = si->x	;
		y = si->y	;
		w = si->w	;
		h = si->h	;
	}

//	printf("msfoof_scale: %'.0f %'.0f, %'.1f %'.1f\n", ti->x, ti->y, w*wr, h*hr);

	// si ihaatah is where to start and end in the s msfoof
	// ti ihaatah is where to start in the t msfoof
	void interpolate(int tx, int ty, int sx, int sy) {
		float r = 0;
		float g = 0;
		float b = 0;
		int num = 0;
		int radius = 1;
		/* Iterate through a bounding box in which the circle lies */
		for (int i = sx - radius; i < sx + radius; i++) {
			for (int j = sy - radius; j < sy + radius; j++) {
				int c = msfoof_get(s, i, j);

				/* If the pixel is outside the canvas, skip it */
				if (c == -1)
				continue;
			
				/* If the pixel is outside the circle, skip it */
				if (distance(sx, sy, i, j) > radius)
				continue;
			
				/* Get the color from the image, add to a running sum */
				lwn l;
				rgb2lwn(c, &l);
//				r += l.r;
//				g += l.g;
//				b += l.b;
//				r += l.r*l.r;
//				g += l.g*l.g;
//				b += l.b*l.b;
				r += l.r*l.r*l.r;
				g += l.g*l.g*l.g;
				b += l.b*l.b*l.b;
				num++;
			}
		}
		/* Return the mean of the R, G, and B components */
//		lwn l = {0, r/num, g/num, b/num};
		lwn l = {0, cbrt(r/num), cbrt(g/num), cbrt(b/num)};
//		lwn l = {0, sqrt(r/num), sqrt(g/num), sqrt(b/num)};
		msfoof_rect( t, tx, ty, wr, hr, -1, lwn2rgb(&l) );
//		msfoof_set( t, tx, ty, lwn2rgb(&l) );


//		for (int i = 0; i < wr; ++i) {
//			for (int j = 0; j < hr; ++j) {
//				int c = msfoof_get(s, sx, sy);
//				int avg = alwaan_avg( lwn2rgb(&l) , c );
//				msfoof_set_blend( t, tx+i, ty+j, avg, .5 );
////				msfoof_set( t, tx+i, ty+j, avg );
//			}
//		}
	}

	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {
			int sx = x+i, sy = y+j; // source x, y
			int tx = ti->x+i*wr, ty = ti->y+j*hr; // target x, y

			c = msfoof_get(s, sx, sy);

//			if (c != -1) msfoof_set_blend( t, tx, ty, c, .2 );
//			if (c != -1) msfoof_set( t, tx, ty, c );
//			if (c != -1) msfoof_rect( t, tx, ty, wr, hr, -1, c );
			if (wr < 1) {
				if (c != -1) msfoof_set( t, tx, ty, c );
			} else
				if (c != -1) msfoof_rect( t, tx, ty, wr, hr, -1, c );
//				interpolate(tx, ty, sx, sy);
		}
	}
}

void matrix_circle(msfoof *m, float x, float y, float r, float sa, float ea, int o, int f) {
	float
		sw = m->strokew,
		incx = m->smooth ? 1.0/m->nmvj.x : 1,
		incy = m->smooth ? 1.0/m->nmvj.y : 1,
		tr = 1.0/(m->nmvj.x*2),
		ea2 = 0, sw2 = sw*3;
	if (sa > 0) {
		if (sa > 360.0) sa = sa-360.0;
		ea = sa+ea;
		if (ea > 360.0) ea2 = fabs(360.0-ea);
	}
	if (sa == ea) return;
	if (f == -1 && o == -1) return;

	float a = 0, d = 0, r2 = r*2,
		xmin = x-r, ymin = y-r, rmin = 0,
		xmax = x+r, ymax = y+r, rmax = r;
	int c, yes = 1;
	if (o != -1 && f == -1) {
		rmin = r;
	}
	if (o != -1) {
		xmin = x-r-sw, ymin = y-r-sw,
		xmax = x+r+sw, ymax = y+r+sw, rmax = r+sw;
	}
	ihaatahf ih = {xmin, ymin, xmax, ymax};
	ihaatah ih2 = m->maqtoo3;
	if (ihaatah_yajid(&ih2)) {
		if (ih.x < ih2.x) ih.x = ih2.x;
		if (ih.y < ih2.y) ih.y = ih2.y;
		if (ih.w > ih2.w) ih.w = ih2.w+ih2.x;
		if (ih.h > ih2.h) ih.h = ih2.h+ih2.y;
		if (ih.x > ih2.w || ih.y > ih2.h) yes = 1;
	}
	if (yes)
	for (float j = ih.x; j < ih.w; j+=incx)
		for (float k = ih.y; k < ih.h; k+=incy) {
			d = distancef(j, k, x, y);
			if (d >= rmin && d <= rmax) {
				if (d > r) c = o;
				else if (d <= r) c = f;
				a = coordtoangle(j, k, x, y);

				if (c != -1)
				if ((a >= sa && a <= ea) || (ea2 && a >= 0 && a <= ea2))
					if (m->smooth/* && d > r-1*/)
						msfoof_set_blend(m, j, k, c, tr);
					else
						msfoof_set(m, j, k, c);
			}
		}
}
void matrix_circle_alif(msfoof *m, float x, float y, float r, float sa, float ea, int c, int f, char taba3) {
	if (sa == ea) return;
	float sw = m->strokew, ea2 = 0, sw2 = sw*m->nmvj.x;
	if (sa > 0) {
		if (sa > 360.0) sa = sa-360.0;
		ea = sa+ea;
		if (ea > 360.0) ea2 = fabs(360.0-ea);
	}
	x *= m->nmvj.x; y *= m->nmvj.x; r *= m->nmvj.x;
	float a = 0, d = 0, r2 = r*2;
	if (c != -1) {
		for (int j = x-r-sw; j <= x+r+sw; j++)
			for (int k = y-r-sw; k <= y+r+sw; k++) {
				d = distancef(j, k, x, y);
				if (d > r && d <= r+sw) {
					a = coordtoangle(j, k, x, y);

					if ((a >= sa && a <= ea) || (ea2 && a >= 0 && a <= ea2))
					msfoof_set_alif(m, j, k, 1);
				}
			}
		if (taba3) {
			ihaatah g = {x-r-sw, y-r-sw, r2+sw2, r2+sw2};
			matrix_print_alpha_ihaatah(m, &g, c);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}

	if (f != -1) {
		for (int j = x-r; j <= x+r; j++)
			for (int k = y-r; k <= y+r; k++) {
				d = distancef(j, k, x, y);
				if (d <= r) {
					a = coordtoangle(j, k, x, y);

					if ((a >= sa && a <= ea) || (ea2 && a >= 0 && a <= ea2))
					msfoof_set_alif(m, j, k, 1);
				}
			}
		
		if (taba3) {
			ihaatah g = {x-r, y-r, r2+sw, r2+sw};
			matrix_print_alpha_ihaatah(m, &g, f);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}
}
void msfoof_ring(msfoof *m, int x, int y, int r, int t, int c) {
	// Bresenham circle algo
	if (c > -1)
	for (int j=x-r; j<=x+r; j++)
		for (int k=y-r; k<=y+r; k++) {
			int d = distance(j, k, x, y);
			if (d >= (r-(t)) && d <= r)
				msfoof_set(m, j, k, c);
		}
}
void msfoof_arc(msfoof *m, int x, int y, int r, int sa, int ea, int c, int s) {
	// center, radius, start-end angle
	if (sa != ea) {
		if (sa > ea) sa = sa - 360;
		if (s) {
			nuqtahf ss, ee;
			angletocoord(x, y, r, sa, &ss);
			angletocoord(x, y, r, ea, &ee);
			msfoof_line(m, ss.x, ss.y, ee.x, ee.y, c);
		} else {
			float lx = -1, ly = -1, hpi = M_PI / 180;
			for (float a = sa; a < ea; a += 1) {
				float na = a-180; // shift angle -180
				float cx = x + r * cosf(na*hpi);
				float cy = y + r * sinf(na*hpi);
				if (lx != -1) {
					msfoof_line(m, cx, cy, lx, ly, c);
				}
				lx = cx;
				ly = cy;
			}
		}
	}
}

void msfoof_curve(msfoof *m, int x0, int y0, int x1, int y1,
					int x2, int y2, u_int color) {
	msfoof_curve_poly(m, x0, y0, x1, y1, x2, y2, color, NULL);
}
void msfoof_curve_poly(msfoof *m, int x0, int y0, int x1, int y1,
						int x2, int y2, u_int color, polygon *poly) {
	// x2, y2 is curve nuqtah
	// if poly is null, and stroke w is >1, then line is actually drawn
	int parts = sqrt( distance(x1, y1, x2, y2) );
	int lastx = x0, lasty = y0;
	float w = m->strokew;
	if (w < 1) w = 1;
	int length = 0;
	if (poly) poly->nuqaat[length] = (nuqtah) { x0, y0 };
	length++;
	for (int t = 0; t < parts; t++){
		float T = (float) t / parts, pow1 = pow(1-T,2), pow2 = pow(T,2);
		int x = pow1 * x0 + 2 * (1-T) * T * x1 + pow2 * x2;
		int y = pow1 * y0 + 2 * (1-T) * T * y1 + pow2 * y2;

		if ( ! ( x == lastx && y == lasty ) ) {
			if (poly) poly->nuqaat[length] = (nuqtah) { x, y };
			length++; //
			if (w && poly == NULL) msfoof_line(m, lastx, lasty, x, y, color);
			lastx = x;
			lasty = y;
		}
	}
	if ( ! ( lastx == x2 && lasty == y2 ) ) {
		if (poly) poly->nuqaat[length] = (nuqtah) { x2, y2 };
		length++;
		if (w && poly == NULL) msfoof_line(m, lastx, lasty, x2, y2, color);
	}
	if (poly) poly->length = length;
}
void msfoof_3atf(msfoof *m, float x0, float y0, float x1, float y1, float x2, float y2, u_int color) {
	// x2, y2 is curve nuqtah
	// if poly is null, and stroke w is >1, then line is actually drawn
	float parts = sqrtf( distancef(x1, y1, x2, y2) ) * 2;
//	float parts = 3;
	float lastx = x0, lasty = y0;
	for (float t = 0; t < parts; t+=.25){
		float T = t / parts, pow1 = powf(1-T,2), pow2 = powf(T,2);
		float x = pow1 * x0 + 2 * (1-T) * T * x1 + pow2 * x2;
		float y = pow1 * y0 + 2 * (1-T) * T * y1 + pow2 * y2;

		if ( ! ( x == lastx && y == lasty ) ) {
			msfoof_satar(m, lastx, lasty, x, y, color);
			lastx = x;
			lasty = y;
		}
	}
	if ( ! ( lastx == x2 && lasty == y2 ) ) {
		msfoof_satar(m, lastx, lasty, x2, y2, color);
	}
}
void msfoof_3atf_alif(msfoof *m, float x0, float y0, float x1, float y1, float x2, float y2, u_int color, char taba3) {
	// x2, y2 is curve nuqtah
	// if poly is null, and stroke w is >1, then line is actually drawn
	ihaatah g = {0};
	float parts = sqrtf( distancef(x1, y1, x2, y2) ) * 2;
//	float parts = 3;
	float lastx = x0, lasty = y0;
	for (float t = 0; t < parts; t+=.25){
		float T = t / parts, pow1 = powf(1-T,2), pow2 = powf(T,2);
		float x = pow1 * x0 + 2 * (1-T) * T * x1 + pow2 * x2;
		float y = pow1 * y0 + 2 * (1-T) * T * y1 + pow2 * y2;

		if ( ! ( x == lastx && y == lasty ) ) {
			msfoof_satar_alif(m, lastx, lasty, x, y, 0, 0, &g);
			lastx = x;
			lasty = y;
		}
	}
	if ( ! ( lastx == x2 && lasty == y2 ) ) {
		msfoof_satar_alif(m, lastx, lasty, x2, y2, 0, 0, &g);
	}

	if (taba3) {
		matrix_print_alpha_ihaatah(m, &g, color);
		msfoof_clear_area_alif(m, 0, &g);
	}
}
void msfoof_triangle(msfoof *m, int x0, int y0, int x1, int y1, int x2, int y2, int c, int f) {
	if (c > -1) {
		msfoof_line(m, x0, y0, x1, y1, c);
		msfoof_line(m, x1, y1, x2, y2, c);
		msfoof_line(m, x2, y2, x0, y0, c);
	}
	if (f > -1) {
		nuqtah poly[] = {
			{ x0, y0 },
			{ x1, y1 },
			{ x2, y2 },
			{ x0, y0 },
		};
		void polycheck(msfoof * m, int x, int y) {
			if (inpolygon(x, y, poly, 4)) {
				msfoof_set(m, x, y, f);
			}
		}
		msfoof_each(m, &polycheck, NULL);
	}
}

void msfoof_line_smooth(msfoof *m, int x0, int y0, int x1, int y1, u_int color, int stroke) {
//	if (m->smooth) {
//		lwnf l; rgb2lwnf(color, &l);
//		cairo_set_source_rgb (m->cr, l.r, l.g, l.b-.01);
//		cairo_set_line_width(m->cr, stroke);
//		cairo_move_to(m->cr, x0, y0);
//		cairo_line_to(m->cr, x1, y1);
//		cairo_stroke_preserve(m->cr);
//		return;
//	}
	int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap_(x0, y0);
		swap_(x1, y1);
	}
	if (x0 > x1) {
		swap_(x0, x1);
		swap_(y0, y1);
	}
//	if (y0 > y1) {
//		swap_(x0, y1);
//		swap_(x1, y0);
//	}

	//compute the slope 
	float dx = x1-x0;
	float dy = y1-y0;
	float gradient = dy / dx; 
	if (dx == 0) gradient = 1;

	float xpxl1 = x0;
	float xpxl2 = x1; 
	float y = y0; 

	if (steep) {
		for (float x = xpxl1; x <=xpxl2; x++) {
			// pixel coverage is determined by fractional 
			// part of y co-ordinate 
			if (x < x1) {
				msfoof_set_blend( m, y,	 x, color, 1 - fmodf(y, 1.0f) ); 
				msfoof_set_blend( m, y-1, x, color, fmodf(y, 1.0f) ); 
			}
			y += gradient;
		}
	} else {
		for (float x = xpxl1; x <=xpxl2; x++) {
			if (x < x1) {
				msfoof_set_blend( m, x, y  , color, 1 - fmodf(y, 1.0f) );
				msfoof_set_blend( m, x, y-1, color, fmodf(y, 1.0f) );
			}
			y += gradient;
		}
	}
}
void msfoof_line_stroke(msfoof *m, int x0, int y0, int x1, int y1, u_int color, int stroke) {
	int dx		= abs(x1 - x0)			,
		dy		= abs(y1 - y0)			,
		sx		= (x0 < x1) ? 1 : -1	,
		sy		= (y0 < y1) ? 1 : -1	,
		err		= dx - dy				;

	char yes = 2;
	if (ihaatah_yajid(&m->maqtoo3)) {
		ihaatah ih = {x0, y0, dx, dy};
		ihaatah ih2 = m->maqtoo3;
		yes = do_rects_overlap(&ih2, &ih);
	}

	if (yes)
	if (stroke <= 1) {
		long count = 0;
		while (1) {
			if (yes == 2 || mushtamil(x0, y0, &m->maqtoo3))
			if (m->dotted) {
				if (count % m->dotted == 0)
					msfoof_set(m, x0, y0, color);
				++count;
			} else {
				msfoof_set(m, x0, y0, color);
			}

			if (x0 == x1 && y0 == y1) break;

			int err2 = 2*err;
			if (err2 > -dy) { err -= dy; x0 += sx; }
			if (err2 <  dx) { err += dx; y0 += sy; }
		}
	} else
	if (stroke >= 2) {
//		int dist = distance(x0, y0, x1, y1);
		
		float ang = coordtoangle( x0, y0, x1, y1 );
		float w = stroke, w2 = w*2;

		nuqtahf a, b, c, d;
		angletocoord(x0, y0, w, ang+90, &a );
		angletocoord(x0, y0, w, ang-90, &b );
		angletocoord(x1, y1, w, ang-90, &c );
		angletocoord(x1, y1, w, ang+90, &d );

		int p = x0 < x1 ? x0 : x1,
			q = y0 < y1 ? y0 : y1,
			r = (dx+w2), s = (dy+w2);

		p = (p-w); q = (q-w);
		
		nuqtah poly[] = {
			{ a.x, a.y },
			{ b.x, b.y },
			{ c.x, c.y },
			{ d.x, d.y },
			{ a.x, a.y },
		};

		void polycheck(msfoof * m, int x, int y) {
			if (inpolygon(x, y, poly, 5)) {
				msfoof_set(m, x, y, color);
			}
		}
		
		ihaatah g = { p, q, r, s };
		
		msfoof_each(m, &polycheck, &g);
	}
}
void msfoof_line(msfoof *m, int x0, int y0, int x1, int y1, u_int color) {
//	if (m->smooth == 1)
//		msfoof_line_smooth(m, x0, y0, x1, y1, color, m->strokew);
		msfoof_line_stroke(m, x0, y0, x1, y1, color, m->strokew);
//	else
//		msfoof_line_stroke(m, x0, y0, x1, y1, color, m->strokew);
}
void msfoof_line_alif(msfoof *m, int x0, int y0, int x1, int y1, u_char color) {
	int dx		= abs(x1 - x0)			,
		dy		= abs(y1 - y0)			,
		sx		= (x0 < x1) ? 1 : -1	,
		sy		= (y0 < y1) ? 1 : -1	,
		err		= dx - dy				;

	long count = 0;
	while (1) {
		msfoof_set_alif(m, x0, y0, color);
		if (x0 == x1 && y0 == y1) break;
		int err2 = 2*err;
		if (err2 > -dy) { err -= dy; x0 += sx; }
		if (err2 <  dx) { err += dx; y0 += sy; }
	}
}
float msfoof_satar(msfoof *m, float x0, float y0, float x1, float y1, int f) {
	float w = m->strokew, sw = w, tr = .875;

	float	dx		= fabsf(x1 - x0)		,
			dy		= fabsf(y1 - y0)		,
			sx		= (x0 < x1) ? .25 : -.25,
			sy		= (y0 < y1) ? .25 : -.25,
			err		= dx - dy				,
			ang		= 0,
			mx = x1 > x0, // is x1 bigger
			my = y1 > y0; // is y1 bigger

	char yes = 2;
	if (ihaatah_yajid(&m->maqtoo3)) {
		ihaatah ih = {x0, y0, dx, dy};
		ihaatah ih2 = m->maqtoo3;
		yes = do_rects_overlap(&ih2, &ih);
	}

	nuqtahf n = {0};
	ang = coordtoangle(x0, y0, x1, y1);

	if (yes)
	while (1) {
		if (yes == 2 || mushtamil(x0, y0, &m->maqtoo3))
		for (float i = -sw; i <= sw; i+=.25) {
			angletocoord(x0, y0, i, ang-90, &n);

			if (yes == 2 || mushtamil(n.x, n.y, &m->maqtoo3))
			if (m->smooth) msfoof_set_blend(m, n.x, n.y, f, tr);
			else		   msfoof_set(m, n.x, n.y, f);

			angletocoord(x0, y0, i, ang+90, &n);

			if (yes == 2 || mushtamil(n.x, n.y, &m->maqtoo3))
			if (m->smooth) msfoof_set_blend(m, n.x, n.y, f, tr);
			else		   msfoof_set(m, n.x, n.y, f);
		}

		if (x0 == x1 && y0 == y1) break;
		if (mx && x0 > x1) break;
		else if (!mx && x0 < x1) break;
		if (my && y0 > y1) break;
		else if (!my && y0 < y1) break;

		float err2 = 2*err;
		if (err2 > -dy) { err -= dy; x0 += sx; }
		if (err2 <  dx) { err += dx; y0 += sy; }
	}

	return ang;
}
float msfoof_satar_alif(msfoof *m, float x0, float y0, float x1, float y1, int f, char taba3, ihaatah *xrj) {
	float w = m->strokew, sw;
	if (xrj == NULL) { // assume nuqaat are alif based
		x0 *= 3; y0 *= 3; x1 *= 3; y1 *= 3; w /= 3;
	}
	sw = w;

	float	dx		= fabsf(x1 - x0)		,
			dy		= fabsf(y1 - y0)		,
			sx		= (x0 < x1) ? .25 : -.25,
			sy		= (y0 < y1) ? .25 : -.25,
			err		= dx - dy				,
			ang		= 0,
			mx = x1 > x0, // is x1 bigger
			my = y1 > y0; // is y1 bigger

	nuqtahf n = {0};
	ihaatah g = {0};
	ang = coordtoangle(x0, y0, x1, y1);

	if (taba3 || xrj) {
		nuqtahf a, b, c, d;
		angletocoord(x0, y0, w, ang+90, &a );
		angletocoord(x0, y0, w, ang-90, &b );
		angletocoord(x1, y1, w, ang-90, &c );
		angletocoord(x1, y1, w, ang+90, &d );

		g.x = ( (x0 < x1 ? x0 : x1) - sw );
		g.y = ( (y0 < y1 ? y0 : y1) - sw );
		g.w = (dx+w + 6);
		g.h = (dy+w + 5);
	}

	while (1) {
		for (float i = -sw; i <= sw; i+=1) {
			angletocoord(x0, y0, i, ang-90, &n);
			msfoof_set_alif(m, n.x, n.y, 1);
			angletocoord(x0, y0, i, ang+90, &n);
			msfoof_set_alif(m, n.x, n.y, 1);
		}

		if (x0 == x1 && y0 == y1) break;
		if (mx && x0 >= x1) break;
		else if (!mx && x0 < x1) break;
		if (my && y0 >= y1) break;
		else if (!my && y0 < y1) break;

		float err2 = 2*err;
		if (err2 > -dy) { err -= dy; x0 += sx; }
		if (err2 <  dx) { err += dx; y0 += sy; }
	}

	if (xrj) {
		ihaatah_ws3(xrj, g.x, g.y, g.w-g.x, g.h-g.y);
	}
	if (taba3) {
		matrix_print_alpha_ihaatah(m, &g, f);
		msfoof_clear_area_alif(m, 0, &g);
	}

	return ang;
}

void msfoof_mustateel(msfoof *m, int x, int y, u_int w, u_int h, long c, long f, char taba3) {
	x *= 3; y *= 3; w *= 3; h *= 3;

	if (f != -1) {
		for (int i = x; i < x+w; ++i)
			for (int j = y; j < y+h; ++j)
				msfoof_set_alif(m, i, j, 1);

		if (taba3) {
			ihaatah g = {x, y, w, h};
			matrix_print_alpha_ihaatah(m, &g, f);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}
	
	if (c != -1) {
		ihaatah g = {x, y, w, h};
		msfoof_satar_alif(m, x    , y    , x+w-1, y    , 1, 0, &g);	// 0
		msfoof_satar_alif(m, x+w-1, y    , x+w-1, y+h-1, 1, 0, &g);	// 1
		msfoof_satar_alif(m, x    , y    , x    , y+h-1, 1, 0, &g);	// 2
		msfoof_satar_alif(m, x    , y+h-1, x+w-1, y+h-1, 1, 0, &g);	// 3

		if (taba3) {
			matrix_print_alpha_ihaatah(m, &g, c);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}
}
void msfoof_rect_g(msfoof *m, ihaatah *g, int c, int f) {
	msfoof_rect(m, g->x, g->y, g->w, g->h, c, f);
}
void msfoof_rect(msfoof *m, int x, int y, u_int w, u_int h, long c, long f) {
	int yes = 1, top = 1, left = 1, right = 1, bottom = 1, x2 = x+w, y2 = y+h;

	if (ihaatah_yajid(&m->maqtoo3)) {
		ihaatah ih = {x, y, w, h};
		ihaatah ih2 = m->maqtoo3;
		yes = do_rects_overlap(&ih2, &ih);
		if (yes) {
			int bx2 = ih2.x+ih2.w, by2 = ih2.y+ih2.h;

			if (x < ih2.x) x = ih2.x, left = 0;
			if (y < ih2.y) y = ih2.y, top  = 0;
			if (x2 > bx2) x2 = bx2, right  = 0;
			if (y2 > by2) y2 = by2, bottom = 0;
		}
	}

	if (yes && f != -1)
		for (int i = x; i < x2; ++i)
			for (int j = y; j < y2; ++j)
				msfoof_set(m, i, j, f);
	
	if (yes && c != -1) {
		if (top   ) msfoof_line( m, x   , y   , x2-1, y   , c ); // top
		if (right ) msfoof_line( m, x2-1, y   , x2-1, y2-1, c ); // right
		if (left  ) msfoof_line( m, x   , y   , x   , y2-1, c ); // left
		if (bottom) msfoof_line( m, x   , y2-1, x2-1, y2-1, c ); // bottom
	}
}
void msfoof_rect_alif(msfoof *m, int x, int y, u_int w, u_int h, u_char f) {
	for (int i = x; i < x+w; ++i)
		for (int j = y; j < y+h; ++j)
			msfoof_set_alif(m, i, j, f);
}

void msfoof_mudalla3(msfoof *m, polygonf *p, int o, int f) {
	int length = p->length, yes = 1;
	if (length) {
		nuqtahf *nqt = p->nuqaat;
		if (f != -1) {
			float minX = nqt[0].x, maxX = nqt[0].x,
				  minY = nqt[0].y, maxY = nqt[0].y,
				  incx = m->smooth ? 1.0/m->nmvj.x : 1,
				  incy = m->smooth ? 1.0/m->nmvj.y : 1,
				  tr = 1.0/(m->nmvj.x*2);

			for (int n = 1; n < length; n++) {
				minX = fmin(nqt[n].x, minX);
				maxX = fmax(nqt[n].x, maxX);
				minY = fmin(nqt[n].y, minY);
				maxY = fmax(nqt[n].y, maxY);
			}

			if (ihaatah_yajid(&m->maqtoo3)) {
				ihaatah ih = {minX, minY, maxX-minX, maxY-minY};
				ihaatah ih2 = m->maqtoo3;
				yes = do_rects_overlap(&ih2, &ih);
				if (yes) {
					int bx2 = ih2.x+ih2.w, by2 = ih2.y+ih2.h;

					if (minX < ih2.x) minX = ih2.x;
					if (minY < ih2.y) minY = ih2.y;
					if (maxX > bx2) maxX = bx2;
					if (maxY > by2) maxY = by2;
				}
			}
			
			int c = 0;
			if (yes)
			for (float y = minY; y < maxY; y+=incy) { // from minY to maxY
				for (float x = minX; x < maxX; x+=incx) { // each vert line
					// continue if x, y are inside the bounding box
					if (x < minX || x > maxX || y < minY || y > maxY) return;

					c = 0;
					int j = length - 1;
					for (int i = 0; i < length; ++i) {
						if ( (x == nqt[i].x) && (y == nqt[i].y) ) {
							// point is a corner
							c = 1;
						} else
						if ( (nqt[i].y > y) != (nqt[j].y > y) ) {
							int slope = (x-nqt[i].x)*(nqt[j].y-nqt[i].y)-(nqt[j].x-nqt[i].x)*(y-nqt[i].y);
							if (slope == 0) {
								// point is on boundary
								c = 1;
							} else
							if ( (slope < 0) != (nqt[j].y < nqt[i].y) ) {
								c = !c;
							}
						}

						j = i;
					}
					if (c) {
						if (m->smooth) msfoof_set_blend(m, x, y, f, tr);
						else 		   msfoof_set(m, x, y, f);
					}
				}
			}
		}
		if (o != -1) {
			for (int n = 1; n < length; n++) {
				msfoof_satar(m, nqt[n-1].x, nqt[n-1].y, nqt[n].x, nqt[n].y, o);
			}
			msfoof_satar(m, nqt[0].x, nqt[0].y, nqt[length-1].x, nqt[length-1].y, o);
		}
	}
}
void msfoof_mudalla3_alif(msfoof *m, polygonf *p, int o, int f, char taba3) {
	int length = p->length;
	if (length) {
		nuqtahf *nqt = p->nuqaat;
		
		float minX = nqt[0].x, maxX = nqt[0].x,
			  minY = nqt[0].y, maxY = nqt[0].y;

		for (int n = 1; n < length; n++) {
			minX = fmin(nqt[n].x, minX);
			maxX = fmax(nqt[n].x, maxX);
			minY = fmin(nqt[n].y, minY);
			maxY = fmax(nqt[n].y, maxY);
		}
		
		int c = 0;
		for (float x = minX; x < maxX; x++) { // each vert line
			for (float y = minY; y < maxY; y++) { // from minY to maxY
				// continue if x, y are inside the bounding box
				if (x < minX || x > maxX || y < minY || y > maxY) return;

				c = 0;
				int j = length - 1;
				for (int i = 0; i < length; ++i) {
					if ( (x == nqt[i].x) && (y == nqt[i].y) ) {
						// point is a corner
						c = 1;
					} else
					if ( (nqt[i].y > y) != (nqt[j].y > y) ) {
						int slope = (x-nqt[i].x)*(nqt[j].y-nqt[i].y)-(nqt[j].x-nqt[i].x)*(y-nqt[i].y);
						if (slope == 0) {
							// point is on boundary
							c = 1;
						} else
						if ( (slope < 0) != (nqt[j].y < nqt[i].y) ) {
							c = !c;
						}
					}

					j = i;
				}
				if (c) msfoof_set_alif(m, x, y, 1);
			}
		}
	}
}

void msfoof_each(msfoof *m, void (*handler)(), ihaatah *g) {
	// handler(msfoof * m, int x, int y, u_int color)
	int x = 0, y = 0, w = m->w, h = m->h;
	if (g != NULL) {
		x = g->x; y = g->y; w = x+g->w; h = y+g->h;
	}
	for (int i = x; i < w; ++i) {
		for (int j = y; j < h; ++j) {
			handler( m, i, j, msfoof_get(m, i, j) );
		}
	}
}
void msfoof_each_alif(msfoof *m, void (*handler)(msfoof*, int, int, u_char), ihaatah *g) {
	int x = 0, y = 0, w = m->wa, h = m->ha;
	if (g != NULL) {
		x = g->x; y = g->y; w = x+g->w; h = y+g->h;
	}
	for (int j = y; j < h; ++j) {
		for (int i = x; i < w; ++i) {
			handler( m, i, j, msfoof_get_alif(m, i, j) );
		}
	}
}
void msfoof_each_alif_owst(msfoof *m, void (*handler)(msfoof*, int, int, float alpha), ihaatah *g) {
	int x = 0, y = 0, w = m->wa, h = m->ha;
	if (g != NULL) {
		x = g->x/3.0; y = g->y/3.0; w = g->w/3.0; h = g->h/3.0;
	}
//	printf("%d, %d, %d, %d\n", x, y, w, h);
//	msfoof_rect(m, x, y, w, h, 0xffff00, -1);
	lwnf l = {0};
	for (int j = y; j < y+h; ++j) {
		for (int i = x; i < x+w; ++i) {
			msfoof_get_owst_alif(m, i, j, &l);
			if (l.a) handler( m, i, j, l.a );
		}
	}
}

void msfoof_clear_area_alif(msfoof *m, u_char alif, ihaatah *g) {
	int x = 0, y = 0, w = m->wa, h = m->ha;
	if (g != NULL) {
		x = g->x; y = g->y; w = g->w; h = g->h;
	}
	msfoof_rect_alif(m, x, y, w, h, alif);
}
void msfoof_clear_area(msfoof *m, u_int color, ihaatah *g) {
	int x = 0, y = 0, w = m->w, h = m->h;
	if (g != NULL) {
		x = g->x; y = g->y; w = g->w; h = g->h;
	}
	if (m->clearfn != NULL) {
		char ret = 0;
		for (int i = x; i < x+w; ++i) {
			for (int j = y; j < y+h; ++j) {
				ret = m->clearfn(m, i, j, color);
				if (ret == 0) msfoof_set(m, i, j, color);
			}
		}
	}
	else {
		msfoof_rect(m, x, y, w, h, -1, color);
	}
}
void msfoof_clear_area_f(msfoof *m, u_int color, ihaatahf *g) {
	if (g) {
		ihaatah g2 = { g->x, g->y, g->w, g->h, g->s, g->i };
		msfoof_clear_area(m, color, &g2);
	} else {
		msfoof_clear(m, color);
	}
}
void msfoof_clear(msfoof *m, u_int color) {
	msfoof_clear_area(m, color, NULL);
}
void msfoof_clear_alif(msfoof *m, unsigned char color) {
	msfoof_clear_area_alif(m, color, NULL);
}

void msfoof_set_barzax(msfoof *m, u_int *b) {
	if (m->barzax != NULL) {
//		printf("old barzax is not empty %p %p\n", m->barzax, b);
		free(m->barzax);
	}
	m->barzax = b;

	m->nmvj.x = NMVJX; m->nmvj.y = NMVJY;
}
void msfoof_radd_barzax_alif(msfoof *m) {
	if (m->barzax == NULL) {
		if (m->alif != NULL) {
//			printf("old alif is not empty %p\n", m->alif);
			free(m->alif);
		}
	} else {
		m->sizea = m->w*m->h*NMVJX*NMVJY;
		m->wa = m->w*NMVJX;
		m->ha = m->h*NMVJY;
	
		u_char *alif = (u_char *) malloc(m->sizea * sizeof(u_char));
//		printf("realloc bytes alif: %ld\n", m->sizea * sizeof(u_char));
		if (alif == NULL) {
			printf("malloc failed! %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		m->alif = alif;
	}
}
void msfoof_resize(msfoof *m, u_int w, u_int h) { // if w|h == -1, uses old values
	m->nmvj.x = NMVJX; m->nmvj.y = NMVJY;

	if (w == -1 && h == -1) { // use old values
		m->stride = m->w * sizeof(u_int);
		m->size = m->w*m->h;
		m->sizea = m->w*m->h*9;
	} else {
		m->stride = m->w * sizeof(u_int);
		m->size = w*h;
		m->w = w;
		m->h = h;

		msfoof_set_barzax(m, NULL);
		msfoof_radd_barzax_alif(m);
		
//		printf("realloc bytes: %ld\n", m->size * sizeof(int));
		if (m->barzax == NULL) {
			int *b = (int *) malloc(m->size * sizeof(int));
			if (b == NULL) {
				printf("malloc failed! %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			msfoof_set_barzax(m, b);
		}
	}
}

void msfoof_ikbar_alif(msfoof *m, msfoof *t, int fg) {
	float cw3 = 10/3.0;
	void ikbar(msfoof * m, int x, int y, u_char v) {
		if (v) msfoof_rect(t, x*cw3, 100+y*cw3, cw3, cw3, -1, fg);
	};

	msfoof_each_alif(m, ikbar, NULL);
}
void matrix_print_alpha(msfoof *m, int fg) {
	lwnf l = {0};
	
	void ikbar(msfoof * m, int x, int y, u_int color) {
		msfoof_get_owst_alif(m, x, y, &l);
//		printf("%d, %d, %.1f\n", x, y, num);
//		if (l.a) msfoof_set_blend(m, x, y, lwnf2rgb(&l), l.a);
		msfoof_set_blend(m, x, y, fg, 1-l.a);
	};

	msfoof_each(m, ikbar, NULL);
}
void matrix_print_alpha_ihaatah(msfoof *m, ihaatah *g, int fg) {
	void ikbar(msfoof * m, int x, int y, float alpha) {
//		printf("%d, %d, %.1f\n", x, y, alpha);
//		if (l.a) msfoof_set_blend(m, x, y, lwnf2rgb(&l), l.a);
		msfoof_set_blend(m, x, y, fg, 1-alpha);
	};

	msfoof_each_alif_owst(m, ikbar, g);
}

msfoof msfoof_new(u_int w, u_int h, u_int *b) {
	msfoof m = { w*h, w, h, 1, 0, 0, b };
//	printf("old barzax: %ld\n", sizeof(*m.barzax));
	msfoof_resize(&m, w, h);
	/*
	 * means the object is copied out of the function into the calling function
	 * The original object is gone, but the copy lives on
	 * */
	return m;
}

