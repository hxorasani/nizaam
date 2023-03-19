taba3("---badaa---");
var nuqaat = [[15, 1], [36, 1], [24, 47], [4, 47]], mowjoodah = 0, total = 0,
	bland = 0x333333, bright = 0xaaaaaa, fill = 1, outline = 1, layyin = 1;

for (var n = 0; n < nuqaat.length; n++) {
	total = polygon.adaaf(nuqaat[n][0]*10, nuqaat[n][1]*10);
}

function now() {
	return new Date().getTime();
}
function b_hajm (hadaf) { // on resize
	msfoof.layyin(hadaf, layyin);
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES) {
		var cw = 10,
			cw3 = 1,
			w = raees.w,
			h = raees.h;

		msfoof.nazzaf(hadaf, 0);

//		if (fill)
		msfoof.maqtoo3(RAEES, w/4, h/4, w/2, h/2);

		polygon.sattar(hadaf, outline ? bright : -1, fill ? bland : -1);

		msfoof.maqtoo3(RAEES, 0, 0, 0, 0);

		var mjdh = polygon.axav(mowjoodah);
		if (mjdh) {
			msfoof.daairah(RAEES,  mjdh.x*cw3,     mjdh.y*cw3   , cw*2, 0, 360, bright, -1);
			msfoof.daairah(RAEES, (mjdh.x*cw3)+1, (mjdh.y*cw3)+1, cw*2, 0, 360, bland, -1);
		}
		for (var n = 0; n < total; n++) {
			mjdh = polygon.axav(n);
			msfoof.daairah(RAEES,   mjdh.x*cw3, 0+mjdh.y*cw3, cw, 0, 360, bland, -1);
			msfoof.daairah(RAEES, 1+mjdh.x*cw3, 1+mjdh.y*cw3, cw, 0, 360, bright, -1);
		}
		
		msfoof.mustateel(RAEES, w/4, h/4, w/2, h/2, bland, -1);

		return 1;
	}
	return 0;
}
function b_lowhah (wm) {
	if (wm.haal) {
		var y = 0;
		if (wm.shift) {
			if (wm.ism == 's') layyin = !layyin, msfoof.layyin(RAEES, layyin), y = 1;
			if (wm.ism == 'j') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)-.25 ), y = 1;
			if (wm.ism == 'l') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)+.25 ), y = 1;
		} else if (wm.alt) {
			var mjdh = polygon.axav(mowjoodah);
			if (wm.ism == 'j') mjdh.x -= 1, y = 1;
			if (wm.ism == 'l') mjdh.x += 1, y = 1;
			if (wm.ism == 'i') mjdh.y -= 1, y = 1;
			if (wm.ism == 'k') mjdh.y += 1, y = 1;
			polygon.ja3al(mowjoodah, mjdh.x, mjdh.y);
		} else {
			if (wm.ism == 'a') {
				var a = polygon.axav(total-1);
				total = polygon.adaaf(a.x+1, a.y+1);
				mowjoodah = total-1;
				y = 1;
			}
			if (wm.ism == 'r') {
				total = polygon.havaf();
				mowjoodah = total-1;
				y = 1;
			}

			if (wm.ism == 'f') fill = !fill, y = 1;

			var mjdh = polygon.axav(mowjoodah);
			if (wm.ism == 'j') mjdh.x -= 3, y = 1;
			if (wm.ism == 'l') mjdh.x += 3, y = 1;
			if (wm.ism == 'i') mjdh.y -= 3, y = 1;
			if (wm.ism == 'k') mjdh.y += 3, y = 1;
			polygon.ja3al(mowjoodah, mjdh.x, mjdh.y);

			if (wm.ism == 'z') {
				mowjoodah--;
				if (mowjoodah < 0) mowjoodah = 0;
				y = 1;
			}
			if (wm.ism == 'x') {
				mowjoodah++;
				if (mowjoodah > total-1) mowjoodah = total-1;
				y = 1;
			}
		}
		return y;
	}
	return 0;
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
