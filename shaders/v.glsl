out vec3 varyingNormalDirection;

in vec4 v_color;
in vec3 v_normal;
in vec3 coord3d;
out vec4 f_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;
uniform float has_color;

out vec4 position;
out vec3 frag_pos;
uniform float vertex_size;
uniform int show_vertices;

in vec2 coord2d;
in vec2 textu2d;

out vec2 TexCoords;
uniform mat3 m_3x3_inv_transp;

void main(void) {
	mat4 vp = projection * view * model * transform;
	float z = show_vertices == 1 ? coord3d.z+.01 : coord3d.z;

	varyingNormalDirection = normalize(m_3x3_inv_transp * v_normal);
	position = projection * view * model * transform * vec4(coord3d.x, coord3d.y, z, 1.0);

	gl_Position = vp * vec4(coord3d.x, coord3d.y, z, 1.0);
	gl_PointSize = vertex_size;
	
	f_color = vec4(
		v_color.x, v_color.y, v_color.z,
		has_color == 1 ? v_color.w : 1
	);
	frag_pos = vec3(view * model * transform * vec4(coord3d.x, coord3d.y, z, 1.0));
	TexCoords = textu2d;
	
}