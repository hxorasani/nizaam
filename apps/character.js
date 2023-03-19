require('apps/scene.js');
require('apps/armature2.js');
require('apps/mesh.js');
require('apps/materials.js');

if (!this.character) {
var character;
;(function(){

	character = {
		since: 0,
		length: 4000,
		frame: 0,
		frame_rate: 12,
		now: 0,
		playing: 0,
		play: function (restart) {
			var _ = this;
			if (restart) _.since = _.now = 0;
			_.playing = 1;
			_.since = new Date().getTime()-_.now;
		},
		pause: function () {
			var _ = this;
			_.playing = 0;
		},
		step: function () {
			var _ = this;
			if (!_.playing) return;
			if (_.since == 0) {
				_.since = new Date().getTime();
			} else {
				_.now = new Date().getTime() - _.since;
				if (_.now > _.length) {
					_.now = _.since = 0;
					hooks.run('character.restart');
				}
			}
		},
		set_time: function (v) {
			this.now = v;
		},
		draw: function () {
			canvas.save();
			var _ = this;
			
			_.frame = Math.floor(_.now / 1000 * _.frame_rate);
			var end = Math.floor(_.length / 1000 * _.frame_rate);

			var w = (window.w / (end+1)), h = 48;
			for (var i = 0; i < end+1; ++i) {
				var x = i*w, y = window.h-h;
				var c = (i <= end ? c_gray : c_dgray) .concat(.7);
				if (i % _.frame_rate == 0) c = c_silver;
				if (_.frame == i) c = c_dwhite;
				canvas.lwn.apply(0, c);
				canvas.rect(x+2, y+h/2, w-2, h/2);
				canvas.fill();
			}
			for (var i = 0; i < end+1; ++i) {
				var x = i*w, y = window.h-h;
				if (i % _.frame_rate == 0) {
					canvas.move_to(x+w/3, y+h/3);
					canvas.lwn.apply(0, c_white);
					canvas.text(parsefloat(i/_.frame_rate, 0)+'s');
				}
			}

			
			canvas.restore();
		},
		pointer: function () {
			
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key], _ = this;
			if (m.state) {
				if (k == 'right' && _.now < _.length) {
					_.now += 1000 / 12, yes = 1;
					if (m.shift) _.now = _.since = _.length;
					if (_.now > _.length) _.now = _.length;
				}
				if (k == 'left' && _.now) {
					_.now -= 1000 / 12, yes = 1;
					if (m.shift) _.now = _.since = 0;
					if (_.now < 0) _.now = 0;
				}
			}
			if (m.state == 1) {
				if (k == 'space') _.playing ? _.pause() : _.play(), yes = 1;
				if (k == 'e') {
					chosen_ease++;
					if (chosen_ease > ease.all.length) chosen_ease = 0;
					yes = 1;
				}
			}
			return yes;
		},
	};

})();

light(0, v3(100, 100), 400, 20);

var indicount = 0, U = 512, chosen_ease = 4, direction = 0;

sgl.set_mode(1);

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

//materials.add( 0xff339921 );
var balls = [];
materials.add( 0xff000000 ); // black
materials.add( 0xffff136f ); // red
materials.add( 0xff0084f0 ); // blue
materials.add( 0xff6e983c ); // green
materials.add( 0xffd35b0a ); // choco orange
materials.add( 0xff3fdada ); // cyan dull
materials.add( 0xff57554d ); // dune
hooks.set('character.restart', function () { direction = !direction; });
function on_bound_window () {
	/*ease.all.forEach(function (o, i) {
		var pct = i/ease.all.length;
		
		materials.add( colors.to_integer(1, 1, 1*pct, 1-pct) );

		var x = i*u2;

		balls[i] = mesh( 0, v3(x - U*0.7, -10, 0), u2*.8 );
	});*/
	var b = balls[0] = mesh('bone');
	b.add_cube().select(b.recent.front ).apply_material(1)
				.select(b.recent.right ).apply_material(2)
				.select(b.recent.left  ).apply_material(3)
				.select(b.recent.top   ).apply_material(4)
				.select(b.recent.bottom).apply_material(5)
				.select(b.recent.back  ).apply_material(6)
				;
	b.select(b.recent.top).translate(0, 100);
	b.select(b.recent.top).scale(.1, 1, .1);

	b.add_cube().select(b.recent.front ).apply_material(6)
				.select(b.recent.right ).apply_material(5)
				.select(b.recent.left  ).apply_material(4)
				.select(b.recent.top   ).apply_material(3)
				.select(b.recent.bottom).apply_material(2)
				.select(b.recent.back  ).apply_material(1)
				;
	b.select(b.recent.bottom).scale(.5, 1, .5);
	b.select(b.recent.top   ).translate(0, -300);
	b.select(b.recent.top   ).scale(.2, 1, .2);

	scene.select(b.uid);
}
function on_paint_window () {
	matrix.clear(0x000000);
	canvas.save();
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);

	canvas.lwn(1, 1, 1);
	canvas.line_width(1);
	
	var magnitude  = 600;
	var magnitude2 = U;
	var start  = direction ? magnitude /2 : -magnitude /2, end  = direction ? -magnitude  : magnitude ;
	var start2 = direction ? magnitude2/2 : -magnitude2/2, end2 = direction ? -magnitude2 : magnitude2;

	balls.forEach(function (o, i) {
		var loc = o.get('location');
		var rot = o.get('rotation');
		loc.y = ease[ ease.all[chosen_ease] ](character.now, start , end , character.length);
		rot.y = ease[ ease.all[chosen_ease] ](character.now, start2, end2, character.length);
		rot.z = ease[ ease.all[chosen_ease] ](character.now, start2, end2, character.length);
	});
	
	character.step();
	scene.draw();
	character.draw();

	var str = parsefloat(character.now/1000, 2)+'s - '+character.frame+'f - '+get_fps()+'fps\n'
			+ (character.playing ? 'playing' : 'paused') + '\n'
			+ ease.all[chosen_ease] + '\n';
	
	text(-10, window.h-96, str, 2);

	canvas.restore();

	materials.draw();
	list.draw();
	menu.draw();

	return character.playing ? 2 : 1;
}
function on_paint_indicator () {
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	canvas.font_size(10);
	
	canvas.lwn(.2, .2, .2);
	canvas.rect(0, indicator.h-20, indicator.w, 20);
	canvas.fill();

	canvas.lwn(1, 1, 1);
	text(-2, 0, 'time\nline\n'+indicount++);

	canvas.restore();
	return 1;
}
function on_pointer(m) {
	var yes = 0;
	if (!yes) yes = menu.pointer(m);
	if (!yes) yes = list.pointer(m);
	if (!yes) yes = scene.pointer(m);
	if (!yes) yes = materials.pointer(m);
	if (!yes) yes = character.pointer(m);
	return yes;
}
function on_keyboard (m) {
	var yes;
	if (!yes) yes = menu.keyboard(m);
	if (!yes) yes = list.keyboard(m);
	if (!yes) yes = scene.keyboard(m);
	if (!yes) yes = materials.keyboard(m);
	if (!yes) yes = character.keyboard(m);
	return yes;
}


}