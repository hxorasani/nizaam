require('apps/core/reset.js');
require('apps/core/v3.js');
require('apps/core/hooks.js');
require('apps/menu.js');
require('apps/materials.js');
//require('apps/models/bone.js');
require('apps/list.js');
require('apps/models/monkey.js');
//require('apps/models/ico.js');

function text(x, y, txt, dir) {
	canvas.save();
	xh = canvas.font_bounds();
	String(txt).split("\n").forEach(function (c, e) {
		var mb = canvas.text(c, 2);
		var ww = dir ? mb.w/2 : -5;
		if (dir == 2) ww = -window.w+mb.w;
		var hh = dir == 2 ? mb.h : 0;
		canvas.lwn(0, 0, 0);
		canvas.move_to(x+1-ww, y+1+((1+e)*(xh.a+xh.d))-hh);
		canvas.text(c);
		canvas.lwn(1, 1, 1);
		canvas.move_to(x-ww, y+((1+e)*(xh.a+xh.d))-hh);
		canvas.text(c);
	});
	canvas.renew_path();
	canvas.restore();
}

var current_model = 0, current_tri = 0, current_vert = 0, action = 0, ready = 0;
var MOVE = 1, ROTATE = 2, SCALE = 3, edit_mode = 0, camera_mode = 0;

//{ basic materials
hooks.set('materials.press', function (m) {
	sgl.tri_material(current_model, current_tri, m.uid);
});
materials.add(0xff6e983c);
materials.add(0xff0084f0);
materials.add(0xffd35b0a);
materials.add(0xffff136f);
materials.add(0xffad40da);
materials.add(0xff3fdada);
//}

var U = 512;

var reset_camera = function () {
	camera.translation(0, 0, -U*5, 0),
	camera.rotation(0, 0, 0, 0);
	camera.scale(U, U, U, 0);
	camera.focal(U*2);
};
reset_camera();

;(function(){
	var uid, mats;
	/*uid = sgl.add();
	list.add(uid, 'one');
	sgl.vert_add	(uid,  U,  -U,   0);
	sgl.vert_add	(uid,  U,   U,   0);
	sgl.vert_add	(uid, -U,   U,   0);
	sgl.vert_add	(uid, -U,  -U,   0);
	sgl.tri_add 	(uid, 0, 1, 2);
	sgl.tri_add 	(uid, 2, 3, 0);
	sgl.tri_material(uid, 0, 5);
	sgl.tri_material(uid, 1, 5);
	sgl.translation (uid, 0, 0, -U*6);*/
	

	/*sgl.add();
	sgl.vert_add(1,  U,  -U,   -U);
	sgl.vert_add(1,  U,   U,   -U);
	sgl.vert_add(1, -U,   U,   -U);
	sgl.vert_add(1, -U,  -U,   -U);
	sgl.tri_add (1, 0, 1, 2);
	sgl.tri_add (1, 2, 3, 0);
	sgl.tri_material(1, 0, 1);
	sgl.translation(1, -U, 0, 0);*/

	uid = sgl.add();
	list.add(uid, 'two');
	sgl.backface(uid, 0);
//	var verts = [-0x200,-0x200,-0x200,-0x200,0x200,-0x200,-0x200,-0x200,0x200,-0x200,0x200,0x200,
//	0x200,-0x200,-0x200,0x200,0x200,-0x200,0x200,-0x200,0x200,0x200,0x200,0x200];
//	var tris = [5,0,4,5,1,0,1,2,0,1,3,2,7,4,6,7,5,4,3,6,2,3,7,6,6,0,2,6,4,0,3,5,7,3,1,5];
//	var verts = boxVertices, tris = boxTriangles;
//	var mats = boxMaterials;
//	var verts = boneVertices, tris = boneTriangles;
	var verts = monkeyVertices, tris = monkeyTriangles;
//	var verts = icoVertices, tris = icoTriangles;
	for (var i = 0; i < verts.length; i+=3) {
		sgl.vert_add(uid, verts[i+0], verts[i+1], verts[i+2]);
	}
	for (var i = 0; i < tris.length; i+=3) {
		sgl.tri_add(uid, tris[i+0], tris[i+1], tris[i+2]);
		if (mats)
		sgl.tri_material(uid, i+0, mats[i+0]),
		sgl.tri_material(uid, i+1, mats[i+1]),
		sgl.tri_material(uid, i+2, mats[i+2]);
	}
	
//	sgl.translation(uid, -U, 0, 0, 0);
//	sgl.scale(uid, -U*.9, -U*.9, -U*.9, 0);
//	sgl.rotation(0, 64, 0, -32);
})();

var MODE = ['textured', 'single', 'smooth', 'sharp', 'barycentric', 'tri_index'];
var ACTION = ['', 'MOVE', 'ROTATE', 'SCALE'];
sgl.set_mode(1);
var ray;

var models = sgl.models();
var last_time = 0, frames_passed = 0, fps = 0;
function get_fps() {
	frames_passed++;
	var diff = new Date().getTime()-last_time;
	if (diff >= 1000) {
		fps = frames_passed;
		frames_passed = 0;
		last_time = new Date().getTime();
	}
	return fps;
}

function draw_triangle(va, vb, vc, s) {
	canvas.line_width(1);
	var offset = s ? .5 : -.5;
	canvas.line(va.x+offset, va.y+offset);
	canvas.line(vb.x+offset, vb.y+offset);
	canvas.line(vc.x+offset, vc.y+offset);
	canvas.line(va.x+offset, va.y+offset);
	canvas.lwn(s ? 1 : .3, s ? .5 : .6, s ? 1 : .3);
	canvas.stroke();
}
function draw_circle(va) {
	canvas.circle(va.x, va.y, 2, 0, 360);
	canvas.lwn(.8, .4, 0);
	canvas.fill(1);
	canvas.lwn(1, .8, .3);
	canvas.stroke();
}
function draw_grid() {
	// TODO fix bug, mapping not accurate
	canvas.save();
	var loc = camera.translation();
	canvas.line_width(1);
	for (var i = -10; i < 10; ++i) {
		var a = sgl.to_screen(i*U, 0,  loc.z+(U*3));
		var b = sgl.to_screen(i*U, 0, -loc.z-(U*3));
		if (a && b && a.x) {
			canvas.line(a.x, a.y);
			canvas.line(b.x, b.y);
			canvas.lwn(i ? .2 : 1, .2, .2);
			canvas.stroke();
		}
	}
	canvas.restore();
}

function on_bound_window() {
	ready = 1;
}
function on_paint_window() {
	matrix.clear(0x111111);
	draw_grid();
	sgl.render();
	canvas.save();
	canvas.xat('Ubuntu Mono');
	canvas.font_size('14');

	var loc = sgl.translation(current_model);
	var rot = sgl.rotation(current_model);
	var sca = sgl.scale(current_model);

	var fog = sgl.fog() ? 'fog ' : '';
	var noise = sgl.noise() ? 'noise ' : '';
	var light = sgl.light() ? 'light ' : '';
	var wire = sgl.wire() ? 'wire ' : '';
	
	var str = get_fps()+' fps '+fog+noise+light+wire+'\n'
			+ 'mode ' + MODE[ sgl.get_mode() ] +'\n'
			+ ACTION[ action ] +'\n'
			+ (loc ? 'loc x'+sgl2unit(loc.x)+' y'+sgl2unit(loc.y)+' z'+sgl2unit(loc.z)+'\n' : '')
			+ (rot ? 'rot x'+sgl2deg(rot.x) +' y'+sgl2deg(rot.y) +' z'+sgl2deg(rot.z)+'\n' : '')
			+ (sca ? 'sca x'+sgl2unit(sca.x) +' y'+sgl2unit(sca.y) +' z'+sgl2unit(sca.z)+'\n' : '')
			;
	
	var o = sgl.get(current_model);
	if (o) {
		str += current_model+' cull'+o.cull+' '+o.verts+' verts '+o.tris+' tris\n';
		var tri = sgl.tri_get(current_model, current_tri);
		var va = sgl.vert_to_screen(current_model, tri.x);
		var vb = sgl.vert_to_screen(current_model, tri.y);
		var vc = sgl.vert_to_screen(current_model, tri.z);

//		str += '    x'+va.x+' y'+va.y+' z'+va.z+' w'+va.w+'\n';
//		str += '    x'+vb.x+' y'+vb.y+' z'+vb.z+' w'+vb.w+'\n';
//		str += '    x'+vc.x+' y'+vc.y+' z'+vc.z+' w'+vc.w+'\n';
		
		draw_triangle(va, vb, vc, 1);
		draw_triangle(va, vb, vc);

		draw_circle(va);
		draw_circle(vb);
		draw_circle(vc);
		
		/*for (var j = 0; j < o.verts; ++j) {
			var v = sgl.vert_get(current_model, j);
			var selv = (current_vert == j ? '*' : '');
			str += '    '+selv+j+' '+v.x+' '+v.y+' '+v.z+'\n';
		}*/
		/*for (var j = 0; j < o.tris; ++j) {
			var v = sgl.tri_get(current_model, j);
			var selv = (current_tri == j ? '*' : '');
			str += '    '+selv+j+'  '+v.x+' '+v.y+' '+v.z+' ('+v.material+')\n';
		}*/
	}

	str += '\n'+sgl.models().length+' models\n';

	var models = sgl.models();
	models.forEach(function (o, i) {
		str += o+(current_model == o ? '*' : ' ')+'> '+i+'\n';
	});
	
	text(0, 0, str);
	
	loc = camera.translation()	;
	rot = camera.rotation()		;
	sca = camera.scale()		;
	var foc = camera.focal()	;
	
	str = (camera_mode ? 'yes ' : '')+'[c] camera control\n'
		+ 'x'+sgl2unit(loc.x)+' y'+sgl2unit(loc.y)+' z'+sgl2unit(loc.z)+' loc\n'
		+ 'x'+sgl2deg(rot.x)+' y'+sgl2deg(rot.y)+' z'+sgl2deg(rot.z)+' rot\n'
		+ 'x'+sgl2unit(sca.x)+' y'+sgl2unit(sca.y)+' z'+sgl2unit(sca.z)+' sca\n'
		+ sgl2unit(foc)+' foc\n'
		+ 'press ` grave for materials\n'
		+ (ray ? 'raycast '+ray.model+' '+ray.tri+'\n' : '')
		;
	text(-4, 12, str, 2);
	
	canvas.restore();

	list.draw();
	materials.draw();
	menu.draw();
	
	rot = sgl.rotation(0);
//	if (rot.x >= U) sgl.rotation(0, --rot.x, 0, 0);
//	if (rot.y >= U) sgl.rotation(0, 0, --rot.y, 0);
//	if (rot.z >= U) sgl.rotation(0, 0, 0, --rot.z);
	sgl.rotation(0, 0, transform.z--, 0);

	return 2;
}
function on_paint_indicator() {
	canvas.save();
	matrix.clear(0x443344);
	canvas.stroke();
	canvas.font_size(20);
	text(indicator.w/2, indicator.h/8, 'sgl', 1);
	canvas.restore();
	return 1;
}
var last_pointer, camera_pan;
function on_pointer(m) {
	var yes;
	if (!yes) yes = menu.pointer(m);
	if (!yes) yes = list.pointer(m);
	if (!yes) yes = materials.pointer(m);
	if (!yes) {
		if (action && last_pointer && !camera_mode) {
			var step = m.alt ? U/256 : (m.shift ? U/128 : U/64)
			var delta = {
				x: m.ctrl ? 0 : (m.x-last_pointer.x),
				y: m.ctrl ? 0 : (last_pointer.y-m.y),
				z: m.ctrl ? (last_pointer.y-m.y)*step : 0
			};
			if (action == MOVE) {
				sgl.translation(current_model, delta.x, delta.y, delta.z); yes = 1;
			} else if (action == ROTATE) {
				sgl.rotation(current_model, delta.y/8, delta.x/8, delta.z/8); yes = 1;
			}
		}
		if (!action && camera_pan && last_pointer) {
			var step = m.ctrl ? U/256 : U/64;
			if (m.shift) {
				var delta = camera.translation();
				delta.x -= (m.x-last_pointer.x)*step;
				delta.y -= (last_pointer.y-m.y)*step;
//				delta.z -= (last_pointer.y-m.y)*step;
				camera.translation(delta.x, delta.y, delta.z, delta.w);
			} else {
				var delta = camera.rotation();
				delta.y += (m.x-last_pointer.x)/2;
				camera.rotation(delta.x, delta.y, delta.z, delta.w);
			}
			yes = 1;
		}
		if (!yes && m.w) {
			var loc = camera.translation();
			if (m.w ==  1) loc.z -= U/8, yes = 1;
			if (m.w == -1) loc.z += U/8, yes = 1;
			yes && camera.translation(loc.x, loc.y, loc.z, loc.w);
		}
		if (!yes) ray = sgl.raycast(m.x, m.y);

		if (!yes && m.alt && m.n == 1 && m.state == -1 && !action) camera_pan = 1;
		if (camera_pan && m.state == 1) camera_pan = 0;

		if (!yes && m.n == 1 && ray && m.state == 1 && !action) {
			current_model = ray.model;
			current_tri = ray.tri;
		}

		if (action && m.n == 1 && m.state == 1) action = 0;

		last_pointer = m;
		yes = 1; // hover
	}
	return yes;
}
var transform = v3(0);
function on_keyboard(m) {
	if (ready < 3) ready++;
	if (ready < 3) return;
	var k = keyism[m.key], yes, step = m.alt ? U/128 : (m.shift ? U/16 : U/64);
	if (!yes) yes = menu.keyboard(m);
	if (!yes) yes = list.keyboard(m);
	if (!yes) yes = materials.keyboard(m);
	if (!yes) {
	if (!m.state) {
		if (m.alt) {
			if (k == 'g') {
				var loc = sgl.translation(current_model);
				sgl.translation(current_model, -loc.x, -loc.y, -loc.z);
				yes = 1;
			}
			if (k == 'r') {
				var rot = sgl.rotation(current_model);
				sgl.rotation(current_model, -rot.x, -rot.y, -rot.z);
				yes = 1;
			}
			if (k == 's') {
				var sca = sgl.scale(current_model);
				sgl.scale(current_model, -sca.x, -sca.y, -sca.z);
				sgl.scale(current_model, U, U, U);
				yes = 1;
			}
		} else
		if (m.shift) {
			if (k == 'n') sgl.noise(-1), yes = 1;
			if (k == 'f') sgl.fog  (-1), yes = 1;
			if (k == 'l') sgl.light(-1), yes = 1;
			if (k == 'z') sgl.wire (-1), yes = 1;
			if (k == 'b') sgl.backface(current_model), yes = 1;
		} else {
			if (k == 'g') transform = v3(0), action = MOVE, yes = 1;
			if (k == 'r') transform = v3(0), action = ROTATE, yes = 1;
			if (k == 's') transform = v3(0), action = SCALE, yes = 1;
			if (k == 'c') camera_mode = 1, action = MOVE, yes = 1;
			if (k == 'l') sgl.look_at(0, 0, 0), yes = 1;
			if (k == 'm') sgl.set_mode(), yes = 1;
			if (k == 'delete') sgl.remove(current_model), current_model--, yes = 1;
			if (k == 'enter') action = camera_mode = 0, yes = 1;
			if (k == 'esc') action = camera_mode = edit_mode = 0, yes = 1;
			if (k == 'backspace') reset_camera(), yes = 1;
		}
	}
	if (m.state) {
		var o = sgl.get(current_model);
		if (!action && !m.shift && m.ctrl) { // camera focal
			var foc = camera.focal();
			if (k == 'down') foc -= U/8, yes = 1;
			if (k == 'up'  ) foc += U/8, yes = 1;
			if (yes) camera.focal(foc);
		} else if (!action && m.ctrl && edit_mode && o) { // selection
			if (k == 'left' && current_model > 0			 ) current_model--, yes = 1;
			if (k == 'right'&& current_model < sgl.models().length-1) current_model++, yes = 1;
			if (k == 'up'   && current_vert > 0				 ) current_vert--, yes = 1;
			if (k == 'down' && current_vert < o.verts-1 	 ) current_vert++, yes = 1;
		} else if (!yes && camera_mode && action) {
			var delta = action == ROTATE ? camera.rotation() : action == SCALE ? camera.scale() : camera.translation();
			if (action == ROTATE) {
				if (k == 'right'   ) delta.y -= step, yes = 1;
				if (k == 'left'    ) delta.y += step, yes = 1;
				if (k == 'down'    ) delta.x -= step, yes = 1;
				if (k == 'up'      ) delta.x += step, yes = 1;
			} else {
				if (k == 'right'   ) delta.x += step, yes = 1;
				if (k == 'left'    ) delta.x -= step, yes = 1;
				if (k == 'down'    ) delta.z -= step, yes = 1;
				if (k == 'up'      ) delta.z += step, yes = 1;
			}
			if (k == 'pagedown') delta.y -= step, yes = 1;
			if (k == 'pageup'  ) delta.y += step, yes = 1;
			if (yes) {
				if (action == ROTATE) camera.rotation(delta.x, delta.y, delta.z, delta.w);
				else if (action == SCALE) camera.scale(delta.y, delta.y, delta.y, delta.y);
				else if (action == MOVE) camera.translation(delta.x, delta.y, delta.z, delta.w);
			}
		} else if (o) {
			if (edit_mode && !action) {
				var v = sgl.vert_get(current_model, current_vert);
				if (v) {
					if (k == 'left'    ) v.x -= step, yes = 1;
					if (k == 'right'   ) v.x += step, yes = 1;
					if (k == 'down'    ) v.y -= step, yes = 1;
					if (k == 'up'      ) v.y += step, yes = 1;
					if (k == 'pagedown') v.z -= step, yes = 1;
					if (k == 'pageup'  ) v.z += step, yes = 1;
					yes && sgl.vert_set(current_model, current_vert, v.x, v.y, v.z);
				}
			} else if (action) {
				if (action) {
					if (k == 'right'   ) transform.x += step, yes = 1;
					if (k == 'left'    ) transform.x -= step, yes = 1;
					if (k == 'down'    ) transform.y -= step, yes = 1;
					if (k == 'up'      ) transform.y += step, yes = 1;
					if (k == 'pagedown') transform.z -= step, yes = 1;
					if (k == 'pageup'  ) transform.z += step, yes = 1;
					if (action == MOVE)
						yes && sgl.translation(current_model, transform.x, transform.y, transform.z);
					if (action == ROTATE)
						yes && sgl.rotation(current_model, transform.y, transform.x, transform.z);
					if (action == SCALE)
						yes && sgl.scale(current_model, transform.y, transform.y, transform.y);
				}
			}
		}
	}
	}
	return yes;
}


