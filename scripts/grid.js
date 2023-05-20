if (!global.grid) {
var grid;
;(function(){
	var count = 2 * 16, opacity = .4, sc = 50;
	var extent = count * sc;
	var primary = MeshBuffer(), secondary = MeshBuffer();

	hooks.set('menu.ready', function () {
		menu.add(0, 'grid');
		menu.add('grid_toggle', 'toggle', function () {
			grid.toggle();
		});
	});

	var c = [.5,.5,.5,opacity], red = [1,.2,.2,.7], green = [0,1,0,.7],
		d = [.3,.3,.3,opacity];

	var index = 0, indexc = 0;
	function push_color(clr) {
		primary.colors[indexc+0] = clr[0];
		primary.colors[indexc+1] = clr[1];
		primary.colors[indexc+2] = clr[2];
		primary.colors[indexc+3] = clr[3];
		primary.colors[indexc+4] = clr[0];
		primary.colors[indexc+5] = clr[1];
		primary.colors[indexc+6] = clr[2];
		primary.colors[indexc+7] = clr[3];
	}
	for (var i = -count; i <= count; i++) {
		primary.verts[index+0] = i*sc;
		primary.verts[index+1] = 0;
		primary.verts[index+2] = -count*sc;

		primary.verts[index+3] = i*sc;
		primary.verts[index+4] = 0;
		primary.verts[index+5] = count*sc;

		push_color(i == 0 ? green : c);

		index  += 6;
		indexc += 8;
	}
	for (var i = -count; i <= count; i++) {
		primary.verts[index+0] = -count*sc;
		primary.verts[index+1] = 0;
		primary.verts[index+2] = i*sc;

		primary.verts[index+3] = count*sc;
		primary.verts[index+4] = 0;
		primary.verts[index+5] = i*sc;

		push_color(i == 0 ? red : c);

		index  += 6;
		indexc += 8;
	}

//	for (var i = -count; i <= count; i++) {
//		if (i % 10 != 0) {
//			secondary.verts .push( i, 0,-count );
//			secondary.verts .push( i, 0, count );
//			secondary.colors.push_array( d );
//			secondary.colors.push_array( d );
//		}
//	}
//	for (var i = -count; i <= count; i++) {
//		if (i % 10 != 0) {
//			secondary.verts .push( -count, 0, i );
//			secondary.verts .push(  count, 0, i );
//			secondary.colors.push_array( d );
//			secondary.colors.push_array( d );
//		}
//	}
	
	primary.upload();
//	secondary.upload();

	var show = 1, default_transform = mat4.create();
	grid = {
		two_dee: 0,
		quad: [
			vec3.fromValues( extent, 0,-extent ),
			vec3.fromValues(-extent, 0,-extent ),
			vec3.fromValues(-extent, 0, extent ),
			vec3.fromValues( extent, 0, extent )
		],
		scale: sc,
		draw: function () { if (!show) return;
			if (grid.two_dee) {
				two_dee_cam.activate();
				two_dee_cam.apply();
			}
			if (scene.lighting) sgl.uniform_1i(uniforms.lighting, 0);
			sgl.uniform_1f(uniforms.has_color, 1);
			sgl.uniform_matrix_4fv (uniforms.model, default_transform);
			var cam = get_active_camera()
			if (grid.two_dee || cam.projection == CAMERA_ORTHOGRAPHIC) {
				if (cam.direction || grid.two_dee) var model = mat4.create();
				if (cam.direction ==  3) mat4.rotateZ(model, model, matrix.to_radians( 90));
				if (cam.direction == -3) mat4.rotateZ(model, model, matrix.to_radians(-90));
				if (cam.direction ==  1 || grid.two_dee) mat4.rotateX(model, model, matrix.to_radians( 90));
				if (cam.direction == -1) mat4.rotateX(model, model, matrix.to_radians(-90));
				if (cam.direction || grid.two_dee) sgl.uniform_matrix_4fv (uniforms.model, model);
			}
			primary  .draw_arrays( );
//			secondary.draw_arrays( );
			if (scene.lighting) sgl.uniform_1i(uniforms.lighting, scene.lighting);
		},
		visible: function () {
			return show;
		},
		toggle: function () {
			show = !show;
		},
		hide: function () {
			show = 0;
		},
		show: function () {
			show = 1;
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key];
			if (m.ctrl && k == 'g') {
				if (!m.state) grid.toggle();
				yes = 1;
			}
			return yes;
		},
	};
})();
}


