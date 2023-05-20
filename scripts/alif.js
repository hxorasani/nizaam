;(function(){
var feel = ['happy', 'sad', 'sorrow', 'joy', 'love', 'anger', 'rage',
	'upset', 'hate', 'envy', 'lust', 'delirious', 'deluded'];
var rand = function () { return Math.random() * .4; };
grid.hide();
feel.forEach(function (o, i) {
	var el = element(o);
	el.bg = color4(rand(), rand(), rand(), 1);
	el.bg_hover = color4.add(el.bg, .2);
	el.bg_pressed = color4(c_white);
	el.fg_pressed = color4(c_black);
	el.pad = .1;
	el.anchor = element.LEFT;
	
	var hold_off = 200*i;
	el.animate = {
		rotation: {
			x: {
				h: hold_off+300,
				s: -50,
				e: 0,
				d: 200,
				t: 'backin'
			},
			y: {
				h: hold_off+200,
				s: 30,
				e: 0,
				d: 300,
				t: 'backin'
			},
		},
		location: {
			x: {
				h: hold_off+100,
				s: 9,
				e: 0,
				d: 300,
				t: 'backout'
			},
			y: {
				h: hold_off,
				s: 0,
				e: i-6,
				d: 200,
			}
		}
	};
});

})();
