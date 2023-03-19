if (!list) {
var list;
;(function(){
	var data = {}, show = 0, held, guid = 0, transparent;
	var updatelength = function () { list.length = Object.keys(data).length; }
	var genuid = function () { return 'list-'+guid++; }
	
	list = {
		selected: 0,
		length: 0,
		visible: function () { return show; },
		transparent: function (t) {
			transparent = t;
		},
		button: function (x, y, w, h, text, state, sep) {
			canvas.save();
			canvas.font_size(15);
			var xh = canvas.font_bounds(), bg = .15, fg = .3, padx = 2;
			
			if (state == -1) bg = .1;
			if (state == 1) bg = .3;
			if (state == 2) bg = .4;
			canvas.lwn(sep ? .1 : bg, bg, bg, transparent ? .9 : 1);
			canvas.rect(x+padx, y, w, h);
			canvas.fill(1);

			if (state == 2) fg = .6, padx += 4;
			canvas.lwn(fg, fg, fg);
			canvas.stroke();

			canvas.lwn(sep ? .1 : 1, 1, 1);
			if (text)
			String(text).split("\n").forEach(function (c, e) {
				var bounds = canvas.text(c, 2);
				canvas.move_to(x+6, y+((e+1)*xh.h));
				canvas.text(c);
			});
			canvas.restore();
		},
		draw: function () {
			if (!show) return;
			
			var w = 140, h = 24,
				y = window.h/2 - (list.selected * h),
				x = 0;
			var oy = y;
			
			canvas.line_width(.5);
			if (!transparent) {
				canvas.lwn(.1, .1, .01);
				canvas.rect(x-2, y-2, w+4, (h*list.length)+4);
				canvas.stroke();
				canvas.renew_path();
			}

			Object.keys(data).forEach(function (c, i) {
				var btn = data[c];
				list.button(x, oy, w, h, btn.text, list.selected == i ? 2 : btn.state, btn.sep);
				btn.x = x; btn.y = oy;
				btn.w = w; btn.h = h;
				oy += h;
			});
			
			canvas.renew_path();
		},
		add: function (uid, text, cb) {
			var sep;
			if (!uid) {
				uid = genuid();
				sep = 1;
			}
			data[uid] = {
				index: list.length,
				text: text || uid,
				cb: cb,
				sep: sep,
			};
			updatelength();
		},
		text: function (uid, text) { // update text if uid exists
			if (data[uid]) {
				data[uid].text = text;
			}
		},
		prev: function () {
			if (list.selected) list.selected--;
//			else list.selected = list.length-1;
		},
		next: function () {
			if (list.selected < list.length-1) list.selected++;
//			else list.selected = 0;
		},
		press: function () {
			if (!show) return;
			var btn;
			Object.keys(data).forEach(function (c, i) { // TODO use .uid matchin
				if (list.selected == i)
					btn = data[c];
			});
			if (btn && btn.cb) {
				btn.cb();
			}
			held = 0;
		},
		remove: function (uid) {
			delete data[uid];
			updatelength();
		},
		empty: function (reset) {
			data = [];
			updatelength();
			if (reset) list.selected = 0;
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
		raycast: function (x, y, state) {
			for (var i in data) {
				var b = data[i];
				b.state = 0;
			}
			for (var i in data) {
				var b = data[i];
				if (matrix.contains(x, y, b.x, b.y, b.w, b.h)) {
					if (state == -1) {
						b.state = -1;
						held = b;
					} else
						b.state = 1;
					
					if (held && state == 1 && held == b) {
						show && held.cb && held.cb();
						list.selected = held.index;
						held = 0;
					}
					
					return 1;
				}
			}
		},
		pointer: function (m) {
			var yes;
			if (m.state == -1 && m.n == 2) {
				yes = 1; // catch to avoid mishaps in other modules
			} else
			if (m.state == 1 && m.n == 2) {
//				list.toggle(), yes = 1;
			} else if (list.visible()) {
				if (m.w) {
					if (m.w == -1) list.prev();
					if (m.w == 1) list.next();
					yes = 1;
				} else {
					yes = list.raycast(m.x, m.y, m.n == 1 ? m.state : undefined);
					if (!yes && m.n == 1)
//						list.toggle(),
						yes = 1; // hide on click outside
				}
			}
			return yes;
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key];
			if (list.visible()) {
				if (!m.state) {
					if (k == 'enter') list.press(), yes = 1;
				}
				if (m.state) {
					if (k == 'up'   ) list.prev(), yes = 1;
					if (k == 'down' ) list.next(), yes = 1;
				}
			}
			if (!m.state) {
				if (k == 'slash') {
					list.toggle(), yes = 1;
				}
			}
			return yes;
		},
	};
	// TODO make a constructor
	// list = fn( options ) returns list {}
})();
}


