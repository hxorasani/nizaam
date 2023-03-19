/* 
 * for now it's all just one global menu that doesn't even support scrolling
 * 
 * TODO
 * change the api to be menu(name), to have multiple searchable menus
 * give easy way to compose+shortcuts
 * also global menu search
 * each item should have a shortcut
 * have a realistic limit to menu items to promote logically grouping actions
 * */
if (!menu) {
var menu;
;(function(){
	var data = {}, show = 0, held, guid = 0;
	var updatelength = function () { menu.length = Object.keys(data).length; }
	var genuid = function () { return 'menu-'+guid++; }
	
	menu = {
		selected: 0,
		length: 0,
		visible: function () { return show; },
		button: function (x, y, w, h, text, state, sep) {
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

			canvas.lwn(sep ? .1 : 1, 1, 1);
			text.split("\n").forEach(function (c, e) {
				var bounds = canvas.text(c, 2);
				canvas.move_to(x+w-bounds.a-padx-4, y+((e+1)*xh.h));
				canvas.text(c);
			});
			canvas.restore();
		},
		draw: function () {
			if (!show) return;
			
			var y = 24,
				w = 160, h = 30,
				x = window.w - w - 2;
			var oy = y;
			
			canvas.line_width(.5);
			canvas.lwn(.1, .1, .1);
			canvas.rect(x-2, y-2, w+4, (h*menu.length)+4);
			canvas.stroke();
			canvas.renew_path();

			Object.keys(data).forEach(function (c, i) {
				var btn = data[c];
				menu.button(x, oy, w, h, btn.text, menu.selected == i ? 2 : btn.state, btn.sep);
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
				index: menu.length,
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
			if (menu.selected) menu.selected--;
			else menu.selected = menu.length-1;
		},
		next: function () {
			if (menu.selected < menu.length-1) menu.selected++;
			else menu.selected = 0;
		},
		press: function () {
			if (!show) return;
			var btn;
			Object.keys(data).forEach(function (c, i) { // TODO use .uid matchin
				if (menu.selected == i)
					btn = data[c];
			});
			if (btn && btn.cb) {
				btn.cb();
			}
			held = show = 0;
		},
		remove: function (uid) {
			delete data[uid];
			updatelength();
		},
		toggle: function () {
			show = !show;
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
						menu.selected = held.index;
						held = show = 0;
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
				menu.toggle(), yes = 1;
			} else if (menu.visible()) {
				if (m.w) {
					if (m.w == 1) menu.prev();
					if (m.w == -1) menu.next();
					yes = 1;
				} else {
					yes = menu.raycast(m.x, m.y, m.n == 1 ? m.state : undefined);
					if (!yes && m.n == 1) menu.toggle(), yes = 1; // hide on click outside
				}
			}
			return yes;
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key];
			if (menu.visible()) {
				if (!m.state) { // released
					if (k == 'enter') menu.press(), yes = 1;
					if (k == 'esc') menu.toggle(), yes = 1;
				}
				if (m.state) { // press or repeat
					if (k == 'up'   ) menu.prev(), yes = 1;
					if (k == 'down' ) menu.next(), yes = 1;
				}
				yes = 1;
			}
			if (!m.state) {
				if (k == 'compose'	) {
					menu.toggle(), yes = 1;
				}
				if (m.ctrl && m.text == 'l') clear();
			}
			return yes;
		},
	};
})();

menu.add('clear-console', 0, function () { clear(); });
menu.add(0, 'app');
menu.add('exit', 0, function () { exit(); });
menu.add('reload', 0, function () { reload(); });
}

