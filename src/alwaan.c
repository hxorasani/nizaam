#include "alwaan.h"

// only for AA vector drawing
int EIGHTBIT[256] = {
0x000000,0x111111,0x222222,0x333333,0x444444,0x555555,0x666666,0x777777,0x888888,0x999999,0xAAAAAA,0xBBBBBB,0xCCCCCC,0xDDDDDD,0xEEEEEE,0xFFFFFF, // 00 gray
0x0F0000,0x2D0000,0x4B0000,0x690000,0x870000,0xA50000,0xC40000,0xE20000,0xFE0101,0xFF1F1F,0xFF3D3D,0xFE5C5C,0xFF7A7A,0xFF9898,0xFEB6B6,0xFFD4D4, // 01 red
0x0F0006,0x2D0013,0x4B0020,0x69002D,0x87003A,0xA50047,0xC40054,0xE20060,0xFE016E,0xFF1F7F,0xFF3D90,0xFE5CA1,0xFF7AB3,0xFF98C4,0xFEB6D5,0xFFD4E6, // 02 gray
0x0F000C,0x2D0026,0x4B0040,0x69005A,0x870074,0xA5008E,0xC400A8,0xE200C1,0xFE01DA,0xFF1FDF,0xFF3DE3,0xFE5CE7,0xFF7AEC,0xFF98F0,0xFEB6F4,0xFFD4F8, // 03 gray
0x0A000F,0x20002D,0x35004B,0x4B0069,0x600087,0x7600A5,0x8C00C4,0xA100E2,0xB601FE,0xBF1FFF,0xC73DFF,0xD05CFE,0xD97AFF,0xE198FF,0xEAB6FE,0xF2D4FF, // 04 gray
0x04000F,0x0C002D,0x15004B,0x1E0069,0x260087,0x2F00A5,0x3800C4,0x4000E2,0x4901FE,0x5F1FFF,0x753DFF,0x8A5CFE,0xA07AFF,0xB598FF,0xCBB6FE,0xE0D4FF, // 05 gray
0x00020F,0x00062D,0x000A4B,0x000F69,0x001387,0x0017A5,0x001CC4,0x0020E2,0x0125FE,0x1F3FFF,0x3D59FF,0x5C73FE,0x7A8DFF,0x98A7FF,0xB6C0FE,0xD4DAFF, // 06 gray
0x00080F,0x00192D,0x002B4B,0x003C69,0x004D87,0x005EA5,0x0070C4,0x0081E2,0x0192FE,0x1F9FFF,0x3DACFF,0x5CB9FE,0x7AC6FF,0x98D3FF,0xB6DFFE,0xD4ECFF, // 07 gray
0x000F0F,0x002D2D,0x004B4B,0x006969,0x008787,0x00A5A5,0x00C4C4,0x00E2E2,0x01FEFE,0x1FFFFF,0x3DFFFF,0x5CFEFE,0x7AFFFF,0x98FFFF,0xB6FEFE,0xD4FFFF, // 08 gray
0x000F08,0x002D19,0x004B2B,0x00693C,0x00874D,0x00A55E,0x00C470,0x00E281,0x01FE92,0x1FFF9F,0x3DFFAC,0x5CFEB9,0x7AFFC6,0x98FFD3,0xB6FEDF,0xD4FFEC, // 09 gray
0x000F02,0x002D06,0x004B0A,0x00690F,0x008713,0x00A517,0x00C41C,0x00E220,0x01FE25,0x1FFF3F,0x3DFF59,0x5CFE73,0x7AFF8D,0x98FFA7,0xB6FEC0,0xD4FFDA, // 10 gray
0x040F00,0x0C2D00,0x154B00,0x1E6900,0x268700,0x2FA500,0x38C400,0x40E200,0x49FE01,0x5FFF1F,0x75FF3D,0x8AFE5C,0xA0FF7A,0xB5FF98,0xCBFEB6,0xE0FFD4, // 12 gray
0x0A0F00,0x202D00,0x354B00,0x4B6900,0x608700,0x76A500,0x8CC400,0xA1E200,0xB6FE01,0xBFFF1F,0xC7FF3D,0xD0FE5C,0xD9FF7A,0xE1FF98,0xEAFEB6,0xF2FFD4, // 13 gray
0x0F0C00,0x2D2600,0x4B4000,0x695A00,0x877400,0xA58E00,0xC4A800,0xE2C100,0xFEDA01,0xFFDF1F,0xFFE33D,0xFEE75C,0xFFEC7A,0xFFF098,0xFEF4B6,0xFFF8D4, // 14 gray
0x0F0600,0x2D1300,0x4B2000,0x692D00,0x873A00,0xA54700,0xC45400,0xE26000,0xFE6E01,0xFF7F1F,0xFF903D,0xFEA15C,0xFFB37A,0xFFC498,0xFED5B6,0xFFE6D4, // 15 gray
};

/*lwnh rgb2lwnh(lwnf *in) {
    hsv         out;
    double      min, max, delta;

    min = in->r < in->g ? in->r : in->g;
    min = min  < in->b ? min  : in->b;

    max = in->r > in->g ? in->r : in->g;
    max = max  > in->b ? max  : in->b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in->r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in->g - in->b ) / delta;        // between yellow & magenta
    else
    if( in->g >= max )
        out.h = 2.0 + ( in->b - in->r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in->r - in->g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}*/

int lwnh2rgb(lwnh *in) {
    double      hh, p, q, t, ff;
    long        i;
    lwnf        out;

    if(in->s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in->v;
        out.g = in->v;
        out.b = in->v;
        return lwnf2rgb(&out);
    }
    hh = in->h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in->v * (1.0 - in->s);
    q = in->v * (1.0 - (in->s * ff));
    t = in->v * (1.0 - (in->s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in->v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in->v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in->v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in->v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in->v;
        break;
    case 5:
    default:
        out.r = in->v;
        out.g = p;
        out.b = q;
        break;
    }
    return lwnf2rgb(&out);     
}
/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float hue2rgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1./6) return p + (q - p) * 6 * t;
	if (t < 1./2) return q;
	if (t < 2./3) return p + (q - p) * (2./3 - t) * 6;
	return p; 
}
/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
int lwnl2rgb(lwnh *in) {
	float	h = in->h,
			s = in->s,
			l = in->v;

	lwn result;
	
	float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	float p = 2 * l - q;
	result.r = hue2rgb(p, q, h + 1./3) * 255;
	result.g = hue2rgb(p, q, h) * 255;
	result.b = hue2rgb(p, q, h - 1./3) * 255;
	
	return lwn2rgb(&result);
}
float haddqanaatf(float q) { // limit channel float
	if (q < 0) q = 0;
	if (q > 1) q = 1;
	return q;
}
float haddqanaatf255(float q) { // limit channel float 255
	if (q < 0) q = 0;
	if (q > 255) q = 255;
	return q;
}
int haddqanaat(int q) { // limit channel
	if (q < 0) q = 0;
	if (q > 255) q = 255;
	return q;
}
unsigned char hl2char (float h, float l) { // hue lum, no sat, 0hue=0sat, else 1sat
//	h = haddqanaatf(h); l = haddqanaatf(l);
	if (h < 0) h = 0; if (h > 15) h = 15;
	if (l < 0) l = 0; if (l > 15) l = 15;
	return (unsigned char) ( (h) * 16) + (l);
}
unsigned char rgb2char (int rgb) {
	
}
int char2rgb (unsigned char a) {
	return EIGHTBIT[a];
}
void argb2lwn (int color, lwn *l) {
	// each hex digit is 4 bits, shift them 6 points to the right
	// e.g 0xff003399 becomes 0xff
	l->a = color >> 24	& 0xff;
	l->r = color >> 16	& 0xff;
	l->g = color >>  8	& 0xff;
	l->b = color 		& 0xff;
}
void rgb2lwn (int color, lwn *l) {
	// each hex digit is 4 bits, shift them 6 points to the right
	// e.g 0xff003399 becomes 0xff
	l->r = color >> 16	& 0xff;
	l->g = color >>  8	& 0xff;
	l->b = color 		& 0xff;
}
void rgb2lwnf (int color, lwnf *l) {
	// each hex digit is 4 bits, shift them 6 points to the right
	// e.g 0xff003399 becomes 0xff
	l->r = (color >> 16	& 0xff) / (float) 0xff;
	l->g = (color >>  8	& 0xff) / (float) 0xff;
	l->b = (color 		& 0xff) / (float) 0xff;
}
int lwn2argb (lwn *l) {
	int argb  = l->a << 24	;
		argb += l->r << 16	;
		argb += l->g << 8	;
		argb += l->b		;
	return argb;
}
int lwnf2rgb (lwnf *l) {
	int r = (255*l->r), g = (255*l->g), b = (255*l->b);
	int rgb  = r << 16	;
		rgb += g << 8	;
		rgb += b		;
	return rgb;
}
u_int lwn2rgb (lwn *l) {
	u_int	rgb  = l->r << 16	;
			rgb += l->g << 8	;
			rgb += l->b			;
	return rgb;
}
// TODO delete these old buggy funcs above, replace by comp funcs below!

void argb_to_components (u_int color, u_char *r, u_char *g, u_char *b, u_char *a) {
	// each hex digit is 4 bits, shift them 6 points to the right
	// e.g 0xff003399 becomes 0xff
	*r = color >> 24	& 0xff;
	*g = color >> 16	& 0xff;
	*b = color >>  8	& 0xff;
	*a = color 			& 0xff;
}
void components_to_argb(u_char r, u_char g, u_char b, u_char a, u_int *argb) {
	*argb  = r << 24		;
	*argb += g << 16		;
	*argb += b << 8			;
	*argb += a				;
}

int argb2rgb (int c) {
	lwn l;
	argb2lwn(c, &l);
	return lwn2rgb(&l);
}
int argb32torgb (u_char *c) {
	lwnf l;

	l.a = (float) c[0] / 255;

	// 0alpha 1red 2green 3blue

	l.r = (float) c[1] / 255;
	l.g = (float) c[2] / 255;
	l.b = (float) c[3] / 255;

	// it has to be swapped here in order to preserve the source
	if (!IS_BIG_ENDIAN) { // then reverse it bgra -> argb
		swap_(l.a, l.b);
		swap_(l.r, l.g);
	}

	// demultiply (aka divide) alpha
	l.r = l.r / l.a;
	l.g = l.g / l.a;
	l.b = l.b / l.a;

	return lwnf2rgb(&l);
}
int argb32_get_a(u_char *c) {
	if (IS_BIG_ENDIAN) { // remains the same
		return c[0];
	} else { // then reverse it bgra -> argb, lil endian is reversed
		return c[3];
	}
}
int alwaan_replace_a (int color, float pct) {
	return
	// negate previous alpha channel value 0xXX000000 
	( color - ( (color >> 24 & 0xff) << 24) )
	+ (
		// intify pct of 255
		(int) ( 0xff * pct )
		// shift it 6 digits to the left
		<< 24
	);
}
int alwaan_a (int color, float pct) {
	int a = (color >> 24 & 0xff);
	return ( color - ( a << 24) ) + ( (int) ( a * pct ) << 24 );
}
int alwaan_r (int color, float pct) {
	int r = (color >> 16 & 0xff);
	return ( color - ( r << 16) ) + ( (int) ( r * pct ) << 16 );
}
int alwaan_g (int color, float pct) {
	int g = (color >> 8 & 0xff);
	return ( color - ( g << 8) ) + ( (int) ( g * pct ) << 8 );
}
int alwaan_b (int color, float pct) {
	int b = (color & 0xff);
	return ( color - b ) + ( (int) ( b * pct ) );
}
int alwaan_mod (int color, float r, float g, float b) {
	lwn l; rgb2lwn(color, &l);
	if (r) l.r = haddqanaat( 255 * (l.r/255.0+r) );
	if (g) l.g = haddqanaat( 255 * (l.g/255.0+g) );
	if (b) l.b = haddqanaat( 255 * (l.b/255.0+b) );
	return lwn2rgb(&l);
}
int alwaan_add (int color0, int color1) {
	lwn l0, l1, added;
	rgb2lwn(color0, &l0);
	rgb2lwn(color1, &l1);
	added.r = l0.r + l1.r;
	added.g = l0.g + l1.g;
	added.b = l0.b + l1.b;
	if (added.r <   0) added.r =   0;
	if (added.g <   0) added.g =   0;
	if (added.b <   0) added.b =   0;
	if (added.r > 255) added.r = 255;
	if (added.g > 255) added.g = 255;
	if (added.b > 255) added.b = 255;
	return lwn2rgb(&added);
}
int alwaan_avg (int color0, int color1) {
	lwn l0, l1, avg;
	rgb2lwn(color0, &l0);
	rgb2lwn(color1, &l1);
	avg.r = ( l0.r + l1.r ) / 2;
	avg.g = ( l0.g + l1.g ) / 2;
	avg.b = ( l0.b + l1.b ) / 2;
	return lwn2rgb(&avg);
}
int alwaan_pct (int color, float pct) {
	// apply pct to all channels except alef
	lwn l;
	argb2lwn(color, &l);
	l.r = (int) l.r * pct;
	l.g = (int) l.g * pct;
	l.b = (int) l.b * pct;
	return lwn2argb(&l);
}
int alwaan_avg_pct(int color0, int color1, float pct) { // avg % of c1 to c2
	return alwaan_avg( color0, alwaan_pct(color1, pct) );
}
int alwaan_add_pct(int color0, int color1, float pct) { // add % of c1 to c2
	return alwaan_add( color0, alwaan_pct(color1, pct) );
}
