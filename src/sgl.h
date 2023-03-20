#ifndef _sgl_h
#define _sgl_h

// TODO explore array.h, if it can simplify uid->rid tracking
// TODO rename to sgl
//{ S3L config defines
#define S3L_FLAT 0
#define S3L_NEAR_CROSS_STRATEGY 3
#define S3L_PERSPECTIVE_CORRECTION 0
#define S3L_SORT 1
#define S3L_Z_BUFFER 2
#define S3L_MAX_TRIANGES_DRAWN 1024
// TODO zbuf requires a fixed res to prealloc itself, fix that somehow
int s3l_max_pixels = 600*750;
#define S3L_PIXEL_FUNCTION s3l_draw_pixel
#define S3L_RESOLUTION_X 600
#define S3L_RESOLUTION_Y 750
//#define S3L_MAX_PIXELS 600*750
void s3l_set_resolution(int x, int y) {
//	S3L_resolutionX = x;
//	S3L_resolutionY = y;
//	S3L_MAX_PIXELS = x * y;
}

#include "3d/small3dlib.min.h" // now include the library

#define MAX_MODELS 4096
#define MAX_TRIANGLES 4096
#define MAX_VERTICES 4096 * 3
// i guess each model might cost us 12+3kB (!)
//}
//{ S3L modes, textures, uvs etc
#define MODE_TEXTUERED 0
#define MODE_SINGLE_COLOR 1
#define MODE_NORMAL_SMOOTH 2
#define MODE_NORMAL_SHARP 3
#define MODE_BARYCENTRIC 4
#define MODE_TRIANGLE_INDEX 5

int mode = 1, wire = 0, light = 0, fog = 0, transparency = 0, noise = 0;
uint32_t previousTriangle = -1;
S3L_Vec4 uv0, uv1, uv2;
S3L_Vec4 toLight;
uint16_t l0, l1, l2;
S3L_Vec4 n0, n1, n2, nt;

S3L_Model3D model;
uint8_t *texture;
S3L_Unit *uvs;
S3L_Unit *normals;
S3L_Index *uvIndices;

#define TEXTURE_W 128
#define TEXTURE_H 128
void sampleTexture(const uint8_t *tex, int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
	u = S3L_wrap(u,TEXTURE_W);
	v = S3L_wrap(v,TEXTURE_H);
	const uint8_t *t = tex + (v * TEXTURE_W + u) * 4;
	*r = *t; t++; *g = *t; t++; *b = *t;
}
//}

S3L_Scene scene = { 0 };
S3L_Model3D scene_models[MAX_MODELS];

//{ add remove get materials and model struct
typedef struct {
	u_int  color;
	u_char light;
	u_char fog  ;
	u_char noise;
} sgl_material_t;
sgl_material_t sgl_materials[256] = { 0 };
u_int sgl_materials_count = 0;
typedef struct {
	char hover_highlight;
	char selectable;
	char selected;
	int rid; // index in scene.models
	S3L_Model3D *model; // if this is null, means model was deleted
	u_int *materials; // material ref index per triangle, max 256 (!)
	char *selection; // index based boolean vertex selection
	S3L_Unit *texcoords; // ...?
	// this is where physics props can also go
} sgl_model_t;
sgl_model_t scene_model_props[MAX_MODELS] = { 0 };
u_int sgl_last_uid = 0; // these run out after MAX_MODELS, need gc TODO

u_int sgl_get_total_materials() {
	return sgl_materials_count;
}
u_int sgl_add_material(u_int color) {
	if ( sgl_get_total_materials() < 256 ) {
		sgl_materials[ sgl_materials_count ].color = color;
		sgl_materials_count++;
	}
	return sgl_get_total_materials();
}
sgl_material_t * sgl_get_material(u_int i) {
	if ( i < sgl_get_total_materials() ) {
		return &sgl_materials[ i ];
	}
	return NULL;
}
void sgl_set_material(u_int i, u_int color) {
	if ( i < sgl_get_total_materials() ) {
		sgl_materials[ i ].color = color;
	}
}
u_int sgl_remove_material(u_int i) {
	if ( i < sgl_get_total_materials() ) {
		// TODO maybe zero out all refs to this materials in all scenes
		
		for (int j = i; j < sgl_get_total_materials()-1; ++j) {
			sgl_materials[j] = sgl_materials[j+1];
		}

		if (sgl_materials_count > 0) sgl_materials_count--;
	}
	return sgl_get_total_materials();
}
//}

//{ add remove get models
int s3l_models() {
	return scene.modelCount;
}
sgl_model_t * s3l_get(int i) {
	if (s3l_models() && i > -1 && i < MAX_MODELS) {
		if (scene_model_props[i].model)
			return &scene_model_props[i];
	}
	return NULL; // error
}
int sgl_rid_to_uid(int rid) {
	int found = 0, count = s3l_models(), i = 0;
	while (i < MAX_MODELS && found < count) {
		if (scene_model_props[i].model) {
			if (scene_model_props[i].rid == rid) {
				return i;
			}
			found++;
		}
		i++;
	}
	return -1;
}
void sgl_remap_rid(int old_rid, int new_rid) {
	int found = 0, count = s3l_models(), i = 0;
	while (i < MAX_MODELS && found < count) {
		if (scene_model_props[i].model) {
			if (scene_model_props[i].rid == old_rid) {
				scene_model_props[i].model = &scene.models[new_rid];
				scene_model_props[i].rid = new_rid;
				return;
			}
			found++;
		}
		i++;
	}
}
int s3l_add() { // allocates memory for max verts and tris
	if (s3l_models() < MAX_MODELS) {
		// dont use s3l_get() here cuz that validates count
		int i = s3l_models();
		S3L_Model3D *model = &scene.models[ i ];
		if (model) {
			S3L_model3DInit(
				calloc(MAX_VERTICES, sizeof(S3L_Unit)), 0,
				calloc(MAX_TRIANGLES, sizeof(S3L_Index)), 0,
				model
			);
			if (!model->vertices ) { perror("s3l_add verts calloc"); exit(EXIT_FAILURE); }
			if (!model->triangles) { perror("s3l_add tris  calloc"); exit(EXIT_FAILURE); }

			sgl_model_t *m = &scene_model_props[sgl_last_uid];

			m->rid = i;
			m->model = model;
			m->materials = calloc(MAX_TRIANGLES, sizeof(u_int));
			m->selection = calloc(MAX_TRIANGLES, sizeof(char));
//			m->texcoords = calloc(MAX_TRIANGLES, sizeof(char));
			if (!m->materials) { perror("s3l_add materials  calloc"); exit(EXIT_FAILURE); }
			if (!m->selection) { perror("s3l_add selection  calloc"); exit(EXIT_FAILURE); }
//			if (!m->texcoords) { perror("s3l_add texcoords  calloc"); exit(EXIT_FAILURE); }

			model->config.backfaceCulling = 0; // none
//			S3L_computeModelNormals(model,m##Normals,0);

			scene.modelCount++;
			return sgl_last_uid++;
		}
	}
	return -1;
}
int s3l_remove(int i) {
	if (s3l_models() && i > -1) {
		sgl_model_t *m = s3l_get(i);
		if (m) {
			S3L_Model3D *model = m->model;
			if (model->vertices) {
				free((void *) model->vertices);
				model->vertices = NULL;
			}
			if (model->triangles) {
				free((void *) model->triangles);
				model->triangles = NULL;
			}
			if (m->materials) {
				free((void *) m->materials);
				m->materials = NULL;
			}
			if (m->selection) {
				free((void *) m->selection);
				m->selection = NULL;
			}
			model->vertexCount = 0;
			model->triangleCount = 0;

			for (int j = m->rid; j < s3l_models()-1; ++j) {
				// find sgl_model with rid j+1 and remap to rid j
				sgl_remap_rid(j+1, j);
				scene_models[j] = scene_models[j+1];
			}

			m->model = NULL;
			m->rid = -1;
			
			if (scene.modelCount > 0) scene.modelCount--;
		}
	}
	return s3l_models();
}
//}

//{ add remove get verts inside a model
void s3l_vert_get(int i, S3L_Index j, S3L_Unit v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_VERTICES) {
			v[0] = m->model->vertices[j+0];
			v[1] = m->model->vertices[j+1];
			v[2] = m->model->vertices[j+2];
		}
	}
}
void s3l_vert_to_screen(int i, S3L_Index j, S3L_Vec4 *result) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_VERTICES) {
			S3L_Vec4 point = {
				.x = m->model->vertices[j+0],
				.y = m->model->vertices[j+1],
				.z = m->model->vertices[j+2]
			};

			S3L_Mat4 matFinal, matCamera;
			S3L_makeCameraMatrix(scene.camera.transform, matCamera);
			S3L_makeWorldMatrix(m->model->transform, matFinal);
			S3L_mat4Xmat4(matFinal, matCamera);

			result->x = point.x;
			result->y = point.y;
			result->z = point.z;
			result->w = S3L_F; // needed for translation 

			S3L_vec3Xmat4(result, matFinal);

			_S3L_mapProjectedVertexToScreen(result, scene.camera.focalLength);
		}
	}
}
int s3l_vert_set(int i, S3L_Index j, S3L_Unit v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_VERTICES) {
			m->model->vertices[j+0] = v[0];
			m->model->vertices[j+1] = v[1];
			m->model->vertices[j+2] = v[2];
			return 1;
		}
	}
	return 0;
}
int s3l_vert_add(int i, S3L_Unit v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		// each vert takes 3 ints
		// vert max mem is always 3 * max_tris
		if (m->model->vertexCount*3 < MAX_VERTICES) {
			s3l_vert_set(i, m->model->vertexCount, v);
			m->model->vertexCount++;
			return m->model->vertexCount;
		}
	}
	return -1;
}
int s3l_vert_remove(int i, S3L_Index j) { // returns new count on success or -1
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (m->model->vertexCount > 0 && j < MAX_VERTICES) {
			for (int j = i; j < m->model->vertexCount-1; j+=3) {
				// shift three back each time
				m->model->vertices[j+0] = m->model->vertices[j+3+0];
				m->model->vertices[j+1] = m->model->vertices[j+3+1];
				m->model->vertices[j+2] = m->model->vertices[j+3+2];
			}
			
			m->model->vertexCount--;
			
			return m->model->vertexCount;
		}
	}
	return -1;
}
//}

//{ add remove get tris inside a model
void s3l_tri_get(int i, S3L_Index j, S3L_Index v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_TRIANGLES) {
			v[0] = m->model->triangles[j+0];
			v[1] = m->model->triangles[j+1];
			v[2] = m->model->triangles[j+2];
		}
	}
}
int s3l_tri_set(int i, S3L_Index j, S3L_Index v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_TRIANGLES) {
			m->model->triangles[j+0] = v[0];
			m->model->triangles[j+1] = v[1];
			m->model->triangles[j+2] = v[2];
			return 1;
		}
	}
	return 0;
}
int s3l_tri_material(int i, S3L_Index j, u_int material) {
//	printf("%d, %d, %d\n", i, j, material);
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (j > -1 && j < MAX_TRIANGLES) {
			m->materials[j+0] = material;
			m->materials[j+1] = material;
			m->materials[j+2] = material;
			return 1;
		}
	}
	return 0;
}
u_int s3l_tri_get_material(int i, S3L_Index j) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
//		printf("%d %p\n", i, m->model);
		if (j > -1 && j < MAX_TRIANGLES) {
			return m->materials[j];
		}
	}
	return 0;
}
int s3l_tri_add(int i, S3L_Index v[3]) {
	sgl_model_t *m = s3l_get(i);
	if (m) {
		// each tri takes 3 ints
		if (m->model->triangleCount*3 < MAX_TRIANGLES) {
			s3l_tri_set(i, m->model->triangleCount, v);
			
			m->model->triangleCount++;
			
			return m->model->triangleCount;
		}
	}
	return -1;
}
int s3l_tri_remove(int i, S3L_Index j) { // returns new count on success or -1
	sgl_model_t *m = s3l_get(i);
	if (m) {
		j *= 3;
		if (m->model->triangleCount > 0 && j < MAX_TRIANGLES) {
			for (int j = i; j < m->model->triangleCount-1; j+=3) {
				// shift three back each time
				m->model->triangles[j+0] = m->model->triangles[j+3+0];
				m->model->triangles[j+1] = m->model->triangles[j+3+1];
				m->model->triangles[j+2] = m->model->triangles[j+3+2];
			}
			
			m->model->triangleCount--;
			
			return m->model->triangleCount;
		}
	}
	return -1;
}
//}

//{ draw pixel and raycast
int pointer_x = -1, pointer_y = -1; S3L_PixelInfo pixel_hit = { 0 };
u_char s3l_raycast_hit = 0; u_char s3l_raycast_mode = 0;
u_int material_color = 0;
void s3l_draw_pixel(S3L_PixelInfo *p) {
	if (s3l_raycast_mode) {
		if (p->x == pointer_x && p->y == pointer_y) {
			pixel_hit.modelIndex    = p->modelIndex   ;
			pixel_hit.triangleIndex = p->triangleIndex;
			s3l_raycast_hit = 1;
		}
		return;
	}
	if (!current_canvas || !current_canvas->set_pixel) return;

	char hover_highlight = 0;
	if (p->triangleID != previousTriangle) {
		/*if (mode == MODE_TEXTUERED) {
			S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);
		} else if (mode == MODE_NORMAL_SHARP) {
			S3L_Vec4 v0, v1, v2;
			S3L_getIndexedTriangleValues(p->triangleIndex,model.triangles,model.vertices,3,&v0,&v1,&v2);
			S3L_triangleNormal(v0,v1,v2,&nt);
			nt.x = S3L_clamp(128 + nt.x / 4,0,255);
			nt.y = S3L_clamp(128 + nt.y / 4,0,255);
			nt.z = S3L_clamp(128 + nt.z / 4,0,255);
		}
		if (light || mode == MODE_NORMAL_SMOOTH) {
			S3L_getIndexedTriangleValues(p->triangleIndex,model.triangles,normals,3,&n0,&n1,&n2);
			l0 = 256 + S3L_clamp(S3L_vec3Dot(n0,toLight),-511,511) / 2;
			l1 = 256 + S3L_clamp(S3L_vec3Dot(n1,toLight),-511,511) / 2;
			l2 = 256 + S3L_clamp(S3L_vec3Dot(n2,toLight),-511,511) / 2;
		}*/

		int uid = sgl_rid_to_uid(p->modelIndex);
		if (uid > -1) {
			sgl_model_t *m = s3l_get(uid);
			if (m) {
				hover_highlight = m->hover_highlight;
			}
			u_int material = s3l_tri_get_material(uid, p->triangleIndex);
			sgl_material_t *mat = sgl_get_material(material);
			if (mat) material_color = mat->color;
		} else {
			material_color = 0xff000000;
		}
		previousTriangle = p->triangleID;
	}
	
	if (wire && p->barycentric[0] != 0 && p->barycentric[1] != 0 && p->barycentric[2] != 0)
		return;
	
	int8_t transparent = 0;
	u_char a = 255, r = 0, g = 0, b = 0;
	argb_to_components( material_color, &a, &r, &g, &b );
	
	switch (mode) {
		/*case MODE_TEXTUERED: {
			S3L_Unit uv[2];
			uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
			uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);
			sampleTexture(texture,uv[0] / 4,uv[1] / 4,&r,&g,&b);
			if (transparency && r == 255 && g == 0 && b == 0) transparent = 1;
			break;
		}*/
		case MODE_SINGLE_COLOR: {
			break;
		}
		case MODE_NORMAL_SMOOTH: {
			S3L_Vec4 n;
			n.x = S3L_interpolateBarycentric(n0.x,n1.x,n2.x,p->barycentric);
			n.y = S3L_interpolateBarycentric(n0.y,n1.y,n2.y,p->barycentric);
			n.z = S3L_interpolateBarycentric(n0.z,n1.z,n2.z,p->barycentric);
			S3L_vec3Normalize(&n);
			r = S3L_clamp(r + n.x / 4, g + n.x / 4, b + n.x / 4);
			g = S3L_clamp(r + n.y / 4, g + n.y / 4, b + n.y / 4);
			b = S3L_clamp(r + n.z / 4, g + n.z / 4, b + n.z / 4);
			break;
		}
		case MODE_NORMAL_SHARP: r = nt.x; g = nt.y; b = nt.z; break;
		case MODE_BARYCENTRIC: {
			r = p->barycentric[0] >> 1;
			g = p->barycentric[1] >> 1;
			b = p->barycentric[2] >> 1;
			break;
		}
		case MODE_TRIANGLE_INDEX: {
			r = S3L_min(p->triangleIndex,255);
			g = r;
			b = r;
		}
		default: break;
	}

	if (s3l_raycast_hit && hover_highlight && pixel_hit.modelIndex == p->modelIndex) {
		int f = 40;
		if (pixel_hit.triangleIndex == p->triangleIndex) f += 40;
		r = S3L_clamp(((int16_t) r) + f, 0, 255);
		g = S3L_clamp(((int16_t) g) + f, 0, 255);
		b = S3L_clamp(((int16_t) b) + f, 0, 255);
	}
	
	if (light) {
		int16_t l = S3L_interpolateBarycentric(l0,l1,l2,p->barycentric);
		r = S3L_clamp((((int16_t) r) * l) / S3L_F,0,255);
		g = S3L_clamp((((int16_t) g) * l) / S3L_F,0,255);
		b = S3L_clamp((((int16_t) b) * l) / S3L_F,0,255);
	}
	
	if (fog && scene.camera.focalLength != 0) {
		int16_t f = ((p->depth - S3L_NEAR) * 255) / (S3L_F * 64);
	
		r = S3L_clamp(((int16_t) r) + f, 0, 255);
		g = S3L_clamp(((int16_t) g) + f, 0, 255);
		b = S3L_clamp(((int16_t) b) + f, 0, 255);
	
		f *= 2;
		if (f > -1) a = 255-S3L_clamp(f, 0, 255);
	}
	
	if (transparency && transparent) {
		S3L_zBufferWrite(p->x,p->y,p->previousZ);
		return;
	}
	
	if (noise)	current_canvas->set_pixel(p->x + rand() % 8, p->y + rand() % 8, a, r, g, b); 
	else		current_canvas->set_pixel(p->x, p->y, a, r, g, b); 
}
S3L_PixelInfo * sgl_raycast(u_int x, u_int y) {
	s3l_raycast_mode = 1;
	s3l_raycast_hit = 0;

	S3L_drawScene(scene);

	s3l_raycast_mode = 0;

	if (s3l_raycast_hit) return &pixel_hit;
	return NULL;
}
void sgl_project_point_to_screen(S3L_Vec4 point, S3L_Vec4 *result) {
	S3L_project3DPointToScreen(point, scene.camera, result);
}
void sgl_look_at(S3L_Vec4 point) {
	S3L_lookAt(point, &scene.camera.transform);
}
void sgl_point_at(S3L_Vec4 point, S3L_Transform3D *point2) {
	S3L_lookAt(point, point2);
}
//}

//{ TODO implement this
void sgl_camera_control() {
    S3L_Vec4 camF, camR;
 
    S3L_rotationToDirections(scene.camera.transform.rotation, 20, &camF, &camR, 0);

		 if ('a') scene.camera.transform.rotation.y += 1;
    else if ('d') scene.camera.transform.rotation.y -= 1;
    else if ('w') scene.camera.transform.rotation.x += 1;
    else if ('s') scene.camera.transform.rotation.x -= 1;

		 if ("up"   ) S3L_vec3Add(&scene.camera.transform.translation, camF);
    else if ("down" ) S3L_vec3Sub(&scene.camera.transform.translation, camF);
    else if ("left" ) S3L_vec3Sub(&scene.camera.transform.translation, camR);
    else if ("right") S3L_vec3Add(&scene.camera.transform.translation, camR);
}
//}

int sgl_init() {
	u_int count = sgl_get_total_materials();
	// have to remove in reverse cuz uid is actively decremented
	for (int i = count-1; i > -1; --i) {
		sgl_remove_material(i);
	}

	count = s3l_models();
	for (int i = count-1; i > -1; --i) {
		s3l_remove(i);
	}

	for (int i = 0; i < MAX_MODELS; ++i) {
		sgl_model_t m = scene_model_props[i];
		m.hover_highlight = 0;
		m.selectable = 0;
		m.selected = 0;
		m.hover_highlight = 0;
		m.rid = -1;
		m.model = NULL;
	}
	sgl_last_uid = 0;


	S3L_sceneInit(scene_models, 0, &scene);

	// shift the camera a little bit so that we can see the triangle
//	scene.camera.transform.translation.z = -S3L_F * 4;
//	scene.camera.transform.translation.y = S3L_F / 2;
	
	return 0; // done!
}

#endif