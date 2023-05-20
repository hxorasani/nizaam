//clear();
[
	'scripts/core.js'		,
	'scripts/glsl.js'		, // include before other gl modules
	'scripts/scene.js'		,
	'scripts/camera.js'		,
	'scripts/collision.js'	,
	'scripts/mesh-buffer.js',
	'scripts/mesh.js'		,
	'scripts/stroke.js'		,
	'scripts/text3d.js'		,
	'scripts/grid.js'		,
	'scripts/menu.js'		,
	'scripts/element.js'	,
	'scripts/animate.js'	,
	'scripts/model.js'		,
].forEach(function (o, i) {
	require(Nizaam.scripts_path+o)
});
Nizaam.ambient = color4(0.06, 0.06, 0.06, 1), is_focussed = 1;
var Pointer = {
	last: v3()
};
var keyboard_held = {};

;(function(){
	
var cur_dirs = Filesystem.dirs('.');
for (var i in cur_dirs) {
	var cur = cur_dirs[i];
	if (cur.type == 'file' && cur.name == 'main.js') {
		require(cur.name);
	}
}

var animation = 3;
var cur_r = 65, cur_h = cur_r/2, cur_c = cur_h-14;
global.on_tick = function () {
	var yes;
	for (var i in keyboard_held) {
		if (isundef(keyboard_held[i].state)) delete keyboard_held[i];
		if (keyboard_held[i].state == 2) {
			hooks.rununtilconsumed('nizaam.keyboard', keyboard_held[i], i, keyboard_held);
			yes = 1;
		}
	}
	hooks.run('nizaam.tick');
	if (yes) composer.damage();
}
global.on_bound_window = function () {
	hooks.run('nizaam.resize', Window.w, Window.h);
}
global.on_paint_window = function  () {
	if (animation > 0) animation--;
	if (animation < 0) animation = 0;

	sgl.use_program(glsl.pre);
	hooks.run('nizaam.frame'); // camera applies glsl uniforms here

	// 0. write picking uids
	sgl.bind(sgl.GL_FRAMEBUFFER, glsl.pb);
	sgl.clear(sgl.GL_COLOR_BUFFER_BIT);

//	sgl.enable(sgl.GL_CULL_FACE);
//	sgl.cull_face(sgl.GL_BACK);
//	sgl.front_face(sgl.GL_CCW);

	sgl.enable(sgl.GL_BLEND);
	sgl.enable(sgl.GL_DEPTH_TEST);
	sgl.uniform_1i(uniforms.picking_mode, 1);
	
	scene.frame_start();
	scene.picking_mode = 1;
	scene.draw();
	scene.picking_mode = 0;
	
	sgl.uniform_1i(uniforms.picking_mode, 0);
	var c4 = Nizaam.ambient;
	sgl.clear_color(c4.r, c4.g, c4.b, c4.a);

	// 1. draw 3d scene
	sgl.bind(sgl.GL_FRAMEBUFFER, glsl.fb);
	sgl.uniform_3f(uniforms.fogColor, c4.r, c4.g, c4.b);

	// TODO optimize
	sgl.clear(sgl.GL_COLOR_BUFFER_BIT | sgl.GL_DEPTH_BUFFER_BIT);

	grid.draw();
	animation = scene.draw();

	scene.frame_end();
	
	// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
	sgl.blit_framebuffer(glsl.fb, glsl.ib, Window.w, Window.h);
	
	// 3. now render quad with scene's visuals as its texture image
	sgl.bind(sgl.GL_FRAMEBUFFER, 0);
//	sgl.clear(sgl.GL_COLOR_BUFFER_BIT | sgl.GL_DEPTH_BUFFER_BIT);

	// 4. draw Screen quad
	sgl.disable(sgl.GL_DEPTH_TEST);

	sgl.use_program(glsl.post);
	sgl.active_texture(sgl.GL_TEXTURE0);
	sgl.bind(sgl.GL_TEXTURE_2D, glsl.scr);
	sgl.bind(sgl.GL_VERTEX_ARRAY, glsl.quad_va);
	sgl.draw_arrays(sgl.GL_TRIANGLES, 6);
	sgl.bind(sgl.GL_VERTEX_ARRAY, 0);

	hooks.run('nizaam.paint', function () { animation = 1; });

	return animation ? 2 : 1;
}
global.on_focus = function (m) {
	is_focussed = m;
	frame_rate(m ? 110 : 12);
	return 1;
}
global.on_drop = function (s) {
	hooks.run('nizaam.drop', s);
}
global.on_touch = function (m) {
	return hooks.rununtilconsumed('nizaam.touch', m);
}
global.on_rotate = function (m) { // TODO x y offset
	return hooks.rununtilconsumed('nizaam.rotate', m);
}
global.on_pinch = function (m) { // TODO x y offset
	return hooks.rununtilconsumed('nizaam.pinch', m);
}
global.on_pointer = function (m) {
	var yes = hooks.rununtilconsumed('nizaam.pointer', m);
	Pointer.last = m;
	Pointer.last.guid = new Date().getTime();
	
	if (!yes) yes = scene.pointer(m);
	return 1;
}
global.on_keyboard = function (m) {
	var yes, k = keyism[m.key];
	if (k) {
		if (isundef(m.state)) delete keyboard_held[k];
		else keyboard_held[k] = m;
	}
	// TODO hooks should have priority
	yes = hooks.rununtilconsumed('nizaam.keyboard', m, k, keyboard_held);
	yes = menu.keyboard(m);
	yes = grid.keyboard(m);
	return 1;
}
global.on_destroy = function () { hooks.run('nizaam.destroy'); } // TODO

})();


