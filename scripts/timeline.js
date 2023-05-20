if (!global.Timeline) {
var Timeline;
;(function(){

	Timeline = {
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
					hooks.run('timeline.restart');
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


}