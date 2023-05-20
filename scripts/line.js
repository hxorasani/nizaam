require('apps/core.js');
var attributes = {}, uniforms = {};
var pre = sgl.create_program("shaders/tex.v.glsl", "shaders/tex.f.glsl");
if (undefined == pre) log('error pre program');

var varray = [], ssbo, vao, N;
varray.push(0.0, -1.0, 0.0, 1.0);
varray.push(1.0, -1.0, 0.0, 1.0);
for (var u=0; u <= 90; u += 10)
{
	var a = u*Math.PI/180.0;
	var c = Math.cos(a), s = Math.sin(a);
	varray.push(c, s, 0.0, 1.0);
}
varray.push(-1.0, 1.0, 0.0, 1.0);
for (var u = 90; u >= 0; u -= 10)
{
	var a = u * Math.PI / 180.0;
	var c = Math.cos(a), s = Math.sin(a);
	varray.push(c-1.0, s-1.0, 0.0, 1.0);
}
varray.push(1.0, -1.0, 0.0, 1.0);
varray.push(1.0, 0.0, 0.0, 1.0);

vao = sgl.vertex_array();
ssbo = sgl.ssbo(new Float32Array(varray));
sgl.bind_ssbo_base(ssbo);
N = varray.length/4 - 2;

uniforms.u_mvp = sgl.get_uniform("u_mvp", pre);
uniforms.u_resolution = sgl.get_uniform("u_resolution", pre);
uniforms.u_thickness = sgl.get_uniform("u_thickness", pre);
sgl.uniform_1f(uniforms.u_thickness, 20.0);

log(varray.length*4);
log(6*(N-1));
for (var i = 0; i < N; i++) {
	log(i, varray[i*4+0], varray[i*4+1], varray[i*4+2]);
}

var animation = 2;
var cur_r = 65, cur_h = cur_r/2, cur_c = cur_h-14;
var project = mat4.create();
function on_bound_window() {
//	log('on_bound_window');
	sgl.viewport(0, 0, window.w, window.h);
	sgl.uniform_2f(uniforms.u_resolution, window.w, window.h);
	var aspect = window.w / window.h;
	mat4.ortho(project, -aspect, aspect, -1.0, 1.0, -10.0, 10.0);
}
sgl.clear_color(.1, .1, .1, 1);
function on_paint_window () {
//	log('on_paint_window');
	if (animation > 0) animation--;
	if (animation < 0) animation = 0;

//	sgl.disable(sgl.GL_BLEND);
//	sgl.disable(sgl.GL_CULL_FACE);
//	sgl.disable(sgl.GL_DEPTH_TEST);
	
	sgl.use_program(pre);

	sgl.uniform_2f(uniforms.u_resolution, window.w, window.h);
	sgl.clear(sgl.GL_COLOR_BUFFER_BIT);

	var model = mat4.create();
//	mat4.translate(model, model, [-0.6, 0.0, 0.0]);
	mat4.scale(model, model, [0.5, 0.5, 1.0]);
	var mvp1 = mat4.create();
	mat4.mul(mvp1, project, model);
	sgl.uniform_matrix_4fv(uniforms.u_mvp, mvp1);
	
//	sgl.polygon_mode(sgl.GL_FRONT_AND_BACK, sgl.GL_LINE);
	sgl.polygon_mode(sgl.GL_FRONT_AND_BACK, sgl.GL_FILL);
	sgl.draw_arrays(sgl.GL_TRIANGLES, 6*(N-1));

	return animation ? 2 : 1;
}


