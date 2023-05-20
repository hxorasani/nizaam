/* 
 * for now it's all just one global menu that doesn't even support scrolling
*/
/* TODO
 * change the api to be menu(name), to have multiple searchable menus
 * give easy way to compose+shortcuts
 * also global menu search
 * each item should have a shortcut
 * have a realistic limit to menu items to promote logically grouping actions
 * */
if (!menu) {
var menu, Menu, Menus = {};
;(function(){
var data = {}, guid = 0, root, last_element_h, started;
var updatelength = function () { menu.length = Object.keys(data).length; }
var genuid = function () { return 'menu-'+guid++; }
function element_type_button(_, w, pad, on_release) {
	_.w  = w;
	_.bg = color4(c_gray);
	_.bg_hover = color4(c_silver);
	_.bg_pressed = color4(c_dwhite);
	_.fg_pressed = color4(c_black);
	_.pad = pad;
	_.on_release = on_release;
	_.anchor = [element.RIGHT, element.BOTTOM];
	_.selectable = 0;
}
hooks.set('element.ready', function () {
	menu_button = element('menu');
	menu_button.collection = Collections.get('2d');
	element_type_button(menu_button, 3, .1, function () {
		menu.toggle();
	});
	menu_button.bg = color4(.2, .3, .5, 1);
	menu_button.bg_hover = color4(.3, .4, .6, 1);
	root = menu_button.append();
	root.selectable = 0;
	root.hidden = 1;
	last_element_h = -menu_button.bounds.h+menu_button.bounds.y;
	menu.add('clear-console', 0, function () { clear(); });
	menu.add(0, 'app');
	menu.add('exit', 0, function () { exit(); });
	menu.add('reload', 0, function () { reload(); });
	hooks.run('menu.ready');
});
function update_layout() {
	var o = v3( two_dee_cam.project(Window.w, Window.h) );
	v3.precision(o, 3);
	menu_button.location.x = o.x;
	if (!started) {
		menu_button.animate = {
			location: {
				x: {
					s: o.x+menu_button.w,
					e: o.x,
					d: 300,
					t: 'elasticout'
				}
			},
		};
		started = 1;
	}
	menu_button.location.y = o.y;
	menu_button.location.z = -two_dee_cam.location[2]-menu_button.w;
}
hooks.set('camera.transform', update_layout);

menu = {
	selected: 0,
	length: 0,
	visible: function () { return !root.hidden; },
	add: function (uid, text, cb) {
		var sep;
		if (!uid) {
			uid = genuid();
			sep = 1;
		}
		var el = root.append(sep ? text : text||uid, v3(0,last_element_h,0));
		element_type_button(el, 3, .1, function () {
			menu.toggle();
			if (cb) cb();
		});
		if (sep) {
			el.bg = color4(c_black);
		}
		last_element_h -= el.bounds.h-el.bounds.y;

		data[uid] = el;
		updatelength();
	},
	text: function (uid, text) { // update text if uid exists
		if (data[uid]) data[uid].text.add(text, 1);
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
		if (root.hidden) return;
		var btn;
		Object.keys(data).forEach(function (c, i) { // TODO use .uid matchin
			if (menu.selected == i)
				btn = data[c];
		});
		if (btn && btn.cb) {
			btn.cb();
		}
		root.hidden = 1;
	},
	remove: function (uid) {
		delete data[uid];
		updatelength();
	},
	toggle: function () {
		var h; // queue hide
		if (root.hidden) root.hidden = 0;
		else h = 1;
		root.animate = {
			location: {
				x: {
					s: h ? 0 : 2,
					e: h ? 2 : 0,
					d: 200,
					t: 'back'+(h ? 'in' : 'out'),
					f: function () {
						if (h) root.hidden = 1;
					}
				},
			}
		};
//		if (!root.hidden) aud.play('/home/nano/Music/sci-fi/sound_ex_machina_Action - Closing Effect Sound.mp3');
	},
	keyboard: function (m) {
		var yes, k = keyism[m.key];
		if (!m.state) { // released
			if (k == 'f5') reload(), yes = 1;
		}
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
			if (k == 'compose') {
				menu.toggle(), yes = 1;
			}
		}
		return yes;
	},
};
})();
}

