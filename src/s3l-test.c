#include "s3l-test.h"
#include "3d.h"

#define XATAA 0

int musheerdrawn = 0, mashghool = 0, px = 0, py = 0, pw = 10, drag = 0;
float rotang = 0, zoom = 1;
char goup = 0, godown = 0, goleft = 0, goright = 0;
int lx = 0, ly = 0;
msfoof *win = NULL;

char drawraees() {
	win = &WJHH.raees.mtrx;
	float w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h;
	int active = 0;
	msfoof_clear(win, 0x111111);

	S3L_newFrame();
	S3L_drawScene(scene);

	return active ? 2 : 1;
}
void s3l_set_pixel(int x, int y, int r, int g, int b) {
	if (win) {
		lwn l = {r, g, b};
		msfoof_set(win, x, y, lwn2rgb(&l));
	}
}
void s3l_draw_pixel(S3L_PixelInfo *p) {
  if (p->triangleID != previousTriangle)
  {
    if (mode == MODE_TEXTUERED)
    {
      S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);
    }
    else if (mode == MODE_NORMAL_SHARP)
    {
      S3L_Vec4 v0, v1, v2;
      S3L_getIndexedTriangleValues(p->triangleIndex,model.triangles,model.vertices,3,&v0,&v1,&v2);

      S3L_triangleNormal(v0,v1,v2,&nt);

      nt.x = S3L_clamp(128 + nt.x / 4,0,255);
      nt.y = S3L_clamp(128 + nt.y / 4,0,255);
      nt.z = S3L_clamp(128 + nt.z / 4,0,255);
    }

    if (light || mode == MODE_NORMAL_SMOOTH)
    {
      S3L_getIndexedTriangleValues(p->triangleIndex,model.triangles,normals,3,&n0,&n1,&n2);

      l0 = 256 + S3L_clamp(S3L_vec3Dot(n0,toLight),-511,511) / 2;
      l1 = 256 + S3L_clamp(S3L_vec3Dot(n1,toLight),-511,511) / 2;
      l2 = 256 + S3L_clamp(S3L_vec3Dot(n2,toLight),-511,511) / 2;
    }

    previousTriangle = p->triangleID;
  }

  if (wire)
    if (p->barycentric[0] != 0 &&
        p->barycentric[1] != 0 &&
        p->barycentric[2] != 0)
      return;

  uint8_t
    r = 0,
    g = 0,
    b = 0;

  int8_t transparent = 0;

  switch (mode)
  {
    case MODE_TEXTUERED:
    {
      S3L_Unit uv[2];

      uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
      uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

      sampleTexture(texture,uv[0] / 4,uv[1] / 4,&r,&g,&b);

      if (transparency && r == 255 && g == 0 && b == 0)
        transparent = 1;

      break;
    }
    case MODE_SINGLE_COLOR:
    {
      r = 128;
      g = 128;
      b = 128;

      break;
    }

    case MODE_NORMAL_SMOOTH:
    {
      S3L_Vec4 n;

      n.x = S3L_interpolateBarycentric(n0.x,n1.x,n2.x,p->barycentric);
      n.y = S3L_interpolateBarycentric(n0.y,n1.y,n2.y,p->barycentric);
      n.z = S3L_interpolateBarycentric(n0.z,n1.z,n2.z,p->barycentric);

      S3L_vec3Normalize(&n);

      r = S3L_clamp(128 + n.x / 4,0,255);
      g = S3L_clamp(128 + n.y / 4,0,255);
      b = S3L_clamp(128 + n.z / 4,0,255);
 
      break;
    }

    case MODE_NORMAL_SHARP:
    {
      r = nt.x;
      g = nt.y;
      b = nt.z;
      break;
    }

    case MODE_BARYCENTRIC:
    {
      r = p->barycentric[0] >> 1;
      g = p->barycentric[1] >> 1;
      b = p->barycentric[2] >> 1;
      break;
    }

    case MODE_TRIANGLE_INDEX:
    {
      r = S3L_min(p->triangleIndex,255);
      g = r;
      b = r;
    }
  
    default:
      break;
  }

  if (light)
  {
    int16_t l = S3L_interpolateBarycentric(l0,l1,l2,p->barycentric);

    r = S3L_clamp((((int16_t) r) * l) / S3L_F,0,255);
    g = S3L_clamp((((int16_t) g) * l) / S3L_F,0,255);
    b = S3L_clamp((((int16_t) b) * l) / S3L_F,0,255);
  }

  if (fog)
  {
    int16_t f = ((p->depth - S3L_NEAR) * 255) / (S3L_F * 64);

    f *= 2;

    r = S3L_clamp(((int16_t) r) + f,0,255);
    g = S3L_clamp(((int16_t) g) + f,0,255);
    b = S3L_clamp(((int16_t) b) + f,0,255);
  }

  if (transparency && transparent) {
    S3L_zBufferWrite(p->x,p->y,p->previousZ);
    return;
  }

  if (noise) s3l_set_pixel(p->x + rand() % 8,p->y + rand() % 8,r,g,b); 
  else s3l_set_pixel(p->x,p->y,r,g,b); 
}
void drawmusheer() {
	if (!musheerdrawn) {
		msfoof *m = &WJHH.mshr.mtrx;
		int w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h;
		musheerdrawn = 1;
	}
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == ISHAARAH) {
//			amr_tb3_waaqi3ah(w);
			char yes = 0;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			
			if (drag) {
				if (m.x > lx) px += (m.x-lx);
				if (m.x < lx) px -= (lx-m.x);

				if (m.y > ly) py += (m.y-ly);
				if (m.y < ly) py -= (ly-m.y);
			}

			if (m.state == -1) {
				drag = 1;
			} else if (m.state == 1) {
				drag = 0;
			}

			lx = m.x; ly = m.y;
			yes = 1;
			if (yes) {
				WJHH.mshr.mutadarrar = 1;
				WJHH.raees.mutadarrar = 1;
				mashghool = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 0; float delta = .125;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.key == KEY_LEFT ) goleft  = 1, yes = 1;
				if (m.key == KEY_RIGHT) goright = 1, yes = 1;
				if (m.key == KEY_UP   ) goup    = 1, yes = 1;
				if (m.key == KEY_DOWN ) godown  = 1, yes = 1;

				if (m.key == KEY_F) { fog = !fog; yes = 1; }
				if (m.key == KEY_L) { light = !light; yes = 1; }
				if (m.key == KEY_N) { noise = !noise; yes = 1; }
				if (m.key == KEY_W) { wire = !wire; yes = 1; }
				if (m.key == KEY_M) { mode++; if (mode > 5) mode = 0; yes = 1; }

				if (m.key == KEY_S) yes = 1;
				if (m.key == KEY_Q) exit(0);

				if (m.shift) {
					if (goup   ) scene.models[0].transform.rotation.x = S3L_min(S3L_F, scene.models[0].transform.rotation.x + 1);
					if (godown ) scene.models[0].transform.rotation.x = S3L_min(S3L_F, scene.models[0].transform.rotation.x - 1);
					if (goleft ) scene.models[0].transform.rotation.y = S3L_min(S3L_F, scene.models[0].transform.rotation.y + 1);
					if (goright) scene.models[0].transform.rotation.y = S3L_min(S3L_F, scene.models[0].transform.rotation.y - 1);
					printf("%d, %d\n", scene.models[0].transform.rotation.x, scene.models[0].transform.rotation.y);
				} else if (m.ctrl) {
					if (goup   ) scene.models[0].transform.translation.x = S3L_min(S3L_F, scene.models[0].transform.translation.x + 5);
					if (godown ) scene.models[0].transform.translation.x = S3L_min(S3L_F, scene.models[0].transform.translation.x - 5);
					if (goleft ) scene.models[0].transform.translation.y = S3L_min(S3L_F, scene.models[0].transform.translation.y + 5);
					if (goright) scene.models[0].transform.translation.y = S3L_min(S3L_F, scene.models[0].transform.translation.y - 5);
				} else if (m.alt) {
					if (goup  ) scene.camera.focalLength = S3L_min(S3L_F * 5, scene.camera.focalLength + 5);
					if (godown) scene.camera.focalLength = S3L_max(S3L_F / 2, scene.camera.focalLength - 5);
				} else {
					if (goup   ) scene.camera.transform.translation.z = S3L_min(S3L_F, scene.camera.transform.translation.z + 5);
					if (godown ) scene.camera.transform.translation.z = S3L_min(S3L_F, scene.camera.transform.translation.z - 5);
					if (goleft ) scene.camera.transform.translation.x = S3L_min(S3L_F, scene.camera.transform.translation.x + 5);
					if (goright) scene.camera.transform.translation.x = S3L_min(S3L_F, scene.camera.transform.translation.x - 5);
				}
				yes = 1;
			} else {
				if (m.key == KEY_LEFT ) goleft  = 0, yes = 1;
				if (m.key == KEY_RIGHT) goright = 0, yes = 1;
				if (m.key == KEY_UP   ) goup    = 0, yes = 1;
				if (m.key == KEY_DOWN ) godown  = 0, yes = 1;
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
				WJHH.mshr.mutadarrar = 1;
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
				s3l_set_resolution(WJHH.raees.mtrx.w, WJHH.raees.mtrx.h);
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					char damage = drawraees();
					if (damage != 2)
						WJHH.raees.mutadarrar = 0;
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	tarmeez_core();

	s3l_init(&scene);

	amr_towheed("s3l-test");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
