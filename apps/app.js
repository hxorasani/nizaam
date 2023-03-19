var text, fontsize = fontsize || 24, cursor = 0;
if (!text) {
	text = [];
	'hi what do you mean'.split('').forEach(function (c, e) {
		text[e] = charcodeat(c);
	});
}

function charcodeat(c) {
	return c.charCodeAt();
}
function pointer2circle () {
	msfoof.nazzaf(hadaf, 0);
	var color = mowdoo3.lahjah;
	if (pt.n == -1) color = 0xffffff;
	if (pt.n == 1) color = 0xff0000;
	if (pt.n == 2) color = 0x00ff00;
	if (pt.n == 3) color = 0x0000ff;
	msfoof.daairah(hadaf, pt.x, pt.y, (pt.w*50)||10, 0, 360, -1, color);
}
function lineplusarc () {
	var pt = {}, first, last;
	pt = wm;
	if (wm.haal == -1) {
		first = first || {};
		first.x = wm.x;
		first.y = wm.y;
	} else
	if (wm.haal == 1) {
		last = last || {};
		last.x = wm.x;
		last.y = wm.y;
		yes = 1;
	} else {
//		first = last = 0;
//		yes = 1;
	}
	canvas.lwn(hadaf, 0, 0, 0);
	canvas.dahan(hadaf);
	if (first && last) {
//		msfoof.nazzaf(hadaf, 0);
		canvas.move_to(hadaf, first.x, first.y);

		canvas.lwn(hadaf, 1, 1, 1);
		canvas.matn(hadaf, first.x+', '+first.y);

		canvas.lwn(hadaf, 0, 1, 0);
//		canvas.satar(hadaf, last.x, last.y);
		canvas.eataf(hadaf, last.x, last.y, 60, 0, 360);
		canvas.stroke(hadaf);

		canvas.lwn(hadaf, 1, .1, 0);
		canvas.eataf(hadaf, last.x, last.y, 60, 0, 360);
		canvas.fill(hadaf);

		canvas.lwn(hadaf, 1, 1, 1);
		canvas.move_to(hadaf, last.x, last.y);
		canvas.matn(hadaf, last.x+', '+last.y);
	} else {
		canvas.move_to(hadaf, pt.x, pt.y);
		canvas.lwn(hadaf, 1, 1, 1);
		canvas.matn(hadaf, pt.x+', '+pt.y);
	}
}
function b_hajm (hadaf) {
	canvas.haal(hadaf, 1); // turn canvas on
	// TODO make setting xat+qadd possible, rn it segfaults :(
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES) {
		canvas.xat(hadaf, "Segoe UI Light");
		canvas.font_size(hadaf, fontsize);
		canvas.lwn(hadaf, 0, 0, 0);
		canvas.dahan(hadaf);
		canvas.lwn(hadaf, 1, 1, 1);
		if (text.length) {
			var lastlineheight = 0, lineheight = 20, x = 0;
			if (text.length) {
				var te = canvas.matn(hadaf, charcodeat('M'), 1);
				if (te) {
					lastlineheight += te.h;
				}
			}
			for (var i = 0; i < text.length; ++i) {
				var c = parseInt(text[i]), br = 0;
				canvas.move_to(hadaf, x, lastlineheight);
				if (i == cursor) canvas.matn(hadaf, charcodeat('_'));
				canvas.move_to(hadaf, x, lastlineheight);

				var te = canvas.matn(hadaf, c, 1);
				if (te) {
					lineheight = Math.max(lineheight, te.h);
					x += te.x;
					if (x+te.w >= raees.w || c == charcodeat('\n')) br = 1;
				}
				if (br) {
					if (lineheight < fontsize) lineheight = fontsize;
					lastlineheight += lineheight;
					x = 0;
					lineheight = 0;
				}
				
				if (c != charcodeat('\n')) {
					canvas.matn(hadaf, c);
				}
			}
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
