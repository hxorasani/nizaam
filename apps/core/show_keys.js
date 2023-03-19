if (!show_keys) {
var show_keys;
;(function(){
	var mouse = {}, keyboard = {};
	
	show_keys = {
		draw: function () {
			var str = '';
			k = keyism[keyboard.key];
			if (keyboard.meta) str += 'meta ';
			if (keyboard.shift) str += 'shift ';
			if (keyboard.ctrl) str += 'ctrl ';
			if (keyboard.alt) str += 'alt ';
			if (k) str += k+' ';
			if (mouse.x) str += 'x'+mouse.x+' ';
			if (mouse.y) str += 'y'+mouse.y+' ';
			text(0, 0, str);
		},
		pointer: function (m) {
			mouse = m;
			return 1;
		},
		keyboard: function (m) {
//			if (m.state || m.ctrl || m.shift || m.alt || m.meta)
			keyboard = m;
			return 1;
		},
	};
})();

//menu.add(0, 'show_keys');
//menu.add('reload', 0, function () { reload(); });
}

