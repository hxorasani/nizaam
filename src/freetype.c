/* freetype: 
 * TODO:
 * 	
 * 	
 * */

#include "freetype.h"

#define XATAA 0

int musheerdrawn = 0, started = 0, mashghool = 0, timess = 0, ftheight2 = 24, asmooth = 1;
struct timespec waqt;
kaatib ktb;
FT_Library	library;
FT_Face		face;
unsigned int previous;

void measure(int charcode) {
	long use_kerning = FT_HAS_KERNING(face);

	int error = 0;
	unsigned int glyph_index = FT_Get_Char_Index( face, (unsigned long) charcode );
//	printf("glyph_index %d\n", glyph_index);

	error = FT_Load_Glyph(face, glyph_index, 0 );
	if (error) errexit("freetype load glyph");
	
//	if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
//		if (asmooth)
//		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
//		else
//		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
//		if (error) errexit("freetype render glyph");
//	}

	FT_GlyphSlot slot = face->glyph;
	
	printf("%d %ld\n", charcode, slot->advance.x >> 6);
}
/*void rendercharcode2(int charcode) {
	long use_kerning = FT_HAS_KERNING(face);

	int error = 0;
	unsigned int glyph_index = FT_Get_Char_Index( face, (unsigned long) charcode );
//	printf("glyph_index %d\n", glyph_index);

	error = FT_Load_Glyph(face, glyph_index, 0 );
	if (error) errexit("freetype load glyph");
	
	if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
		if (asmooth)
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		else
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
		if (error) errexit("freetype render glyph");
	}

	FT_GlyphSlot slot = face->glyph;

	FT_Pos bearingY = slot->metrics.horiBearingY >> 6;
	if (shiftY < bearingY) {
//		shiftY = bearingY;
	}

	// retrieve kerning distance and move pen position
	if (use_kerning && previous && glyph_index) {
		FT_Vector delta;
		FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
		pen_x += delta.x >> 6;
	}

	previous = glyph_index;
}*/
/*void renderstring(msfoof *r, int offx, int offy, int *str) {
	// do up an array of glyphs, so u can set max bearingY
	pen_x = 0, shiftY = 0, previous = 0;
	int w = r->w, h = r->h;
	int length = 256;
	int line = 0;
//	FT_Glyph *glyphs = malloc(sizeof(FT_Glyph) * length);  // glyphs table

	for (int i = 0; i < length; ++i) {
		rendercharcode2(str[i]);
		FT_GlyphSlot slot = face->glyph;

		FT_Bitmap bmp = slot->bitmap;
		int ox = slot->bitmap_left;
		int oy = slot->bitmap_top;
		
		if ((pen_x + (slot->advance.x >> 6)) > w) {
			pen_x = 0;
			line++;
			shiftY = (line) * ftheight2;
		}

		ihaatah g = { 0, 0, bmp.width, bmp.rows };
		void hh( msfoof *m, int x, int y, int c ) {
			if (asmooth) {
				u_char value = bmp.buffer[bmp.width * y + x];
				float br = 1.0f-(float) value/255;
				msfoof_set_blend(m, offx+pen_x+ox+x, offy+shiftY+y-oy, 0xffffff, br );
			} else {
				u_char *row = &bmp.buffer[bmp.pitch * y];
				u_char value = row[(x >> 3)];
				char exists = (value & (128 >> (x & 7))) == 0;
				if (!exists) {
					msfoof_set(m, offx+pen_x+ox+x, offy+shiftY+y-oy, 0xffffff );
				}
			}
		}
		
		if (str[i] > 0) msfoof_each(r, &hh, &g);
//		msfoof_rect(r, pen_x, shiftY, slot->advance.x >> 6, ftheight2, 0x338800, -1);
		

		pen_x += slot->advance.x >> 6;
//		pen_x += ftheight2;
	}
}*/
void drawraees() {
	msfoof *mshr = &WJHH.mshr.mtrx;
	msfoof *r = &WJHH.raees.mtrx;

//	ihaatah glbl = {0, 0, r->w, ftheight2*2, 3};
//	msfoof_clear_area(r, 0x333333, &glbl);
	msfoof_clear(r, 0x333333);

	float mw = WJHH.mshr.mtrx.w, mh = WJHH.mshr.mtrx.h;

	#define total 256
	int offset = 1;
	int str[total] = { 0 };
	for (int i = offset; i < total-offset; ++i) {
		str[i-offset] = fushatarmeez.tounicode[i];
	}
	
//	printf("%s\n", str);
	
//	renderstring(r, 0, 0, str);

	started = 1;
}
void drawmusheer() {
	if (!musheerdrawn) {
		musheerdrawn = 1;
		msfoof *m = &WJHH.mshr.mtrx;
		msfoof_clear(m, 0);
	}
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == LOWHAH && !WJHH.raees.mutadarrar && !WJHH.mshr.mutadarrar) {
			char yes = 0; float delta = .25;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.shift) {
					if (m.key == KEY_J) ftheight2-=5, yes = 1;
					if (m.key == KEY_K) ftheight2+=5, yes = 1;
				} else {
					if (m.key == KEY_J) ftheight2--, yes = 1;
					if (m.key == KEY_K) ftheight2++, yes = 1;
				}

				if (m.key == KEY_J || m.key == KEY_K) {
					if (ftheight2 < 1) ftheight2 = 1;
					if (ftheight2 > 256) ftheight2 = 256;
					int error = FT_Set_Pixel_Sizes(face, 0, ftheight2);
					if (error) errexit("freetype set pixel sizes");
				}
				
				if (m.key == KEY_S) {
					asmooth = !asmooth;
					yes = 1;
				}

				if (m.key == KEY_Q) exit(0);
			}
			if (yes) {
				WJHH.mshr.mutadarrar = 1;
				WJHH.raees.mutadarrar = 1;
				mashghool = 1;
			}
		}
		if (w->ism == MUSHEER) {
			if (w->miftaah == MST3D) {
				WJHH.mshr.mtrx.smooth = 1;
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				if (WJHH.mshr.mutadarrar) {
					WJHH.mshr.mutadarrar = 0;
					drawmusheer();
					amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
				}
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				WJHH.raees.mtrx.smooth = 1;
				WJHH.raees.mutadarrar = 1;
			}
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
	int error = FT_Init_FreeType( &library );
	if (error) errexit("freetype init");

	error = FT_New_Face( library,
					argv[1],
//					"/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
					0,
					&face );
	if ( error == FT_Err_Unknown_File_Format )
		errexit("freetype unknown file format");
	else if ( error )
		errexit("freetype broken file");
	
//	printf("num_faces       : %ld\n", face->num_faces);
//	printf("num_glyphs      : %ld\n", face->num_glyphs);
//	printf("units_per_EM    : %d\n", face->units_per_EM);
//	printf("num_fixed_sizes : %d\n", face->num_fixed_sizes);
//	printf("available_sizes : %p\n", face->available_sizes);

	// w, h in pts; device w, h res in dpi
//	error = FT_Set_Char_Size(face, 0, 16*64, 96, 0);
//	if (error) errexit("freetype set char size");

	error = FT_Set_Pixel_Sizes(face, 0, ftheight2);
	if (error) errexit("freetype set pixel sizes");


//	printf("%d %d %d %d\n",
//		slot->bitmap_left, slot->bitmap_top,
//		slot->bitmap.width, slot->bitmap.rows
//	);

//	return 0;

	tarmeez_core();

	#define total 256
	int offset = 1;
	int str[total] = { 0 };
	for (int i = offset; i < total-offset; ++i) {
//		str[i-offset] = fushatarmeez.tounicode[i];
		measure( fushatarmeez.tounicode[i] );
	}
//
//	amr_towheed("anti");
//	amr_wijhah(1);
//
//	amr_axath(&handler);

    return EXIT_SUCCESS;
}
