/* this is more like a panel, it remains open unless closed manually
 * */
if (!materials) {
var materials;
;(function(){
	var data = {}, show = 0, held, guid = 0;
	var genuid = function () { return 'materials-'+guid++; }
	
	materials = {
		selected: 0,
		visible: function () { return show; },
		button: function (x, y, w, h, text, state, sep, color) {
			canvas.save();
			canvas.font_size(18);
			var xh = canvas.font_bounds(), bg = .2, fg = .3, padx = 2;
			
			if (state == 2) padx = 6;
			
			if (state == -1) bg = .1;
			if (state == 1) bg = .3;
			if (state == 2) bg = .4;
			canvas.lwn(sep ? .1 : bg, bg, bg);
			canvas.rect(x-padx, y, w, h);
			canvas.fill(1);

			if (state == 2) fg = .6, padx += 4;
			canvas.lwn(fg, fg, fg);
			canvas.stroke();

			var comp = colors.to_components(color);
			canvas.lwn(comp.r, comp.g, comp.b);
			canvas.rect(x-padx-10, y, 40, h);
			canvas.fill();

			canvas.lwn(sep ? .1 : 1, 1, 1);
			String(text).split("\n").forEach(function (c, e) {
				var bounds = canvas.text(c, 2);
				canvas.move_to(x+w-bounds.a-padx-4, y+((e+1)*xh.h));
				canvas.text(c);
			});
			canvas.restore();
		},
		draw: function () {
			if (!show) return;
			
			var y = 24, w = 160, h = 24, x = window.w - w - 2, oy = y;
			
			canvas.line_width(.5);
			canvas.lwn(.1, .1, .1);
			canvas.rect(x-2, y-2, w+4, (h*sgl.materials_count())+4);
			canvas.stroke();
			canvas.renew_path();

			for (var i = 0; i < sgl.materials_count(); ++i) {
				var m = sgl.materials_get(i);
				if (m) {
					data[i] = Object.assign(data[i] || {}, m);
					var btn = data[i];
					materials.button(x, oy, w, h, i, materials.selected == i ? 2 : btn.state, btn.sep, m.color);
					btn.x = x; btn.y = oy;
					btn.w = w; btn.h = h;
					oy += h;
				}
			}
			
			canvas.renew_path();
		},
		add: function (color) {
			sgl.materials_add(color);
		},
		set: function (i, color) {
			sgl.materials_set(i, color);
		},
		prev: function () {
			if (materials.selected) materials.selected--;
			else materials.selected = sgl.materials_count()-1;
		},
		next: function () {
			if (materials.selected < sgl.materials_count()-1) materials.selected++;
			else materials.selected = 0;
		},
		press: function () {
			if (!show) return;
			var m = sgl.materials_get( materials.selected );
			if (m) hooks.run('materials.press', m);
		},
		remove: function (uid) {
			sgl.materials_remove(uid);
		},
		toggle: function () {
			show = !show;
		},
		raycast: function (x, y, state) {
			for (var i = 0; i < sgl.materials_count(); ++i) {
				var b = data[i];
				if (b) b.state = 0;
			}
			for (var i = 0; i < sgl.materials_count(); ++i) {
				var b = data[i];
				if (matrix.contains(x, y, b.x, b.y, b.w, b.h)) {
					if (state == -1) {
						b.state = -1;
						held = b;
					} else
						b.state = 1;
					
					if (held && state == 1 && held == b) {
						materials.selected = i;
						show && materials.press();
//						show = 0;
					}
					
					return 1;
				}
			}
		},
		pointer: function (m) {
			var yes;
			if (m.state == -1 && m.n == 3) { // mid btn
				yes = 1;
			}
//			else if (m.state == 1 && m.n == 3) { materials.toggle(), yes = 1; }
			else if (materials.visible()) {
				if (m.w) {
					if (m.w == 1) materials.prev();
					if (m.w == -1) materials.next();
					yes = 1;
				} else {
					yes = materials.raycast(m.x, m.y, m.n == 1 ? m.state : undefined);
				}
			}
			return yes;
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key];
			if (materials.visible()) {
				if (k == 'enter' || k == 'esc') yes = 1;
				if (!m.state) { // released
					if (k == 'enter') materials.press(), yes = 1;
					if (k == 'esc') materials.toggle(), yes = 1;
				}
				if (m.state) { // press or repeat
					if (k == 'up'   ) materials.prev(), yes = 1;
					if (k == 'down' ) materials.next(), yes = 1;
				}
				yes = 1;
			}
			if (!m.state) {
				if (k == 'grave') {
					materials.toggle(), yes = 1;
				}
			}
			return yes;
		},
	};
})();
}

