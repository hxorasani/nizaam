if (!show_keys) {
var show_keys;
;(function(){
	var mouse = {}, keyboard = {};
	
	show_keys = {
		draw: function () {
			var str = '', k = keyism[keyboard.key];
			if (keyboard.meta) str += 'meta ';
			if (keyboard.shift) str += 'shift ';
			if (keyboard.ctrl) str += 'ctrl ';
			if (keyboard.alt) str += 'alt ';
			if (k) str += k+' ';
			var m = mouse;
			var camera = get_active_camera();
			if (camera) v3.precision( m = v3(two_dee_cam.project(mouse.x, mouse.y)), 2 );
			if (mouse.x) str += 'x'+mouse.x+' ';
			if (mouse.y) str += 'y'+mouse.y+' ';
			if (m.x) str += '(x'+m.x+') ';
			if (m.y) str += '(y'+m.y+') ';
			if (m.z) str += '(z'+m.z+') ';
			text(5, 2, str);
		},
		pointer: function (m) {
			v3.precision(m, 1);
			mouse = m;
			return 1;
		},
		keyboard: function (m) {
			keyboard = m;
			return 1;
		},
	};
})();
}

