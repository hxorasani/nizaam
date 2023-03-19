var text, fontsize = 64, cursor = 0;
//if (!text) {
	text = [];
	'Mڈط[]لgy\nMgy'.split('').forEach(function (c, e) {
		text[e] = charcodeat(c);
	});
//}

function charcodeat(c) {
	return c.charCodeAt();
}
function b_hajm (hadaf) {
	canvas.haal(1); // turn canvas on
	// TODO make setting xat+qadd possible, rn it segfaults :(
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES) {
		canvas.xat("Segoe UI Light");
		canvas.font_size(fontsize);
		canvas.lwn(0, 0, 0);
		canvas.dahan();
		if (text.length) {
			var xh = canvas.font_bounds();
			var lastlineheight = xh.a-xh.d, lineheight = 20, x = 0,
				ow = raees.w/4,
				oh = raees.h/5,
				ox = ow, oy = oh;

			canvas.line_width(.5);
			canvas.lwn(1, 1, 1);
			for (var i = 0; i < text.length; ++i) {
				var c = parseInt(text[i]), br = 0;
				canvas.move_to(ox+x, oy+lastlineheight);
				
				var te = canvas.matn(c, 1);
				if (te) {
					canvas.rect(ox+x+te.x+.5, oy+lastlineheight-(te.h)+.5, te.w, te.h+xh.d);
					canvas.move_to(ox+x, oy+lastlineheight);

					if (i == cursor) {
						canvas.lwn(.3, .3, 1);
						canvas.move_to(ox+x+te.a-te.x, oy+lastlineheight);
						canvas.matn(charcodeat('|'));
						canvas.lwn(1, 1, 1);
					}

					lineheight = Math.max(lineheight, te.h);
					x += te.a;
					if (x+te.w >= raees.w || c == charcodeat('\n')) br = 1;

				}

				canvas.move_to(ox+x-te.a, oy+lastlineheight);

				if (br) {
					if (lineheight < fontsize) lineheight = fontsize;
					lastlineheight += lineheight;
					x = 0;
					lineheight = 0;
				}
				
				if (c != charcodeat('\n')) {
					canvas.matn(c);
				}
			}
			canvas.lwn(0, 1, 0);
			canvas.stroke();
			canvas.renew_path();
			
			canvas.lwn(1, 0, 0);
			canvas.line_width(1);
			canvas.rect(ox+.5, oy+.5, raees.w-ow-1, raees.h-oh-1);
			//canvas.rect(10, 10, 10, 10);
			canvas.stroke();
			canvas.renew_path();
		}
		return 1;
	}
	if (hadaf == MUSHEER) {
		return 1;
	}
}
function b_icaarah (wm) {
	var yes;
	return yes;
}
function matn_adaaf(c) {
	text = text.slice(0, cursor+1).concat( c , text.slice(cursor+1) );
	cursor++;
	return 1;
}
function b_lowhah (wm) {
	var yes;
	if (wm.haal) {
		var k = keyism[wm.key];
		if (k == 'left') {
			if (cursor > -1) cursor--; yes = 1;
		} else if (k == 'right') {
			if (cursor < text.length-1) cursor++; yes = 1;
		} else if (wm.ctrl && k == 'minus') {
			if (fontsize > 1) fontsize-=.5; yes = 1;
		} else if (wm.ctrl && k == 'equal') {
			if (fontsize < 256) fontsize+=.5; yes = 1;
		} else if (wm.harf && wm.harf.length) {
			wm.harf.split('').forEach(function (c, e) {
				yes = matn_adaaf(charcodeat(c));
			});
		} else if (k == 'enter') {
			yes = matn_adaaf(charcodeat('\n'));
		} else if (k == 'space') {
			yes = matn_adaaf(charcodeat(' '));
		} else if (k == 'backspace') {
			if (cursor > -1) {
				text = text.slice(0, cursor).concat( text.slice(cursor+1) );
				cursor--;
			}
		}
	}
	return yes;
}

//taba3('global: {');
//for (var i in global) {
//	if (typeof global[i] == 'object') {
//		taba3( '  '+i+': {');
//		for (var j in global[i]) {
//			taba3( '    '+j+' '+typeof global[i][j]+' '+global[i][j] );
//		}
//		taba3('  }');
//	} else {
//		taba3( '  '+i+' '+global[i] );
//	}
//}
//taba3('}');
