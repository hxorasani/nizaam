var nuqaat = [[15, 1], [36, 1], [24, 47], [4, 47]], mowjoodah = 0, total = 0,
	bland = 0x333333, bright = 0xaaaaaa, fill = 1, outline = 1, layyin = 1,
	eqadd = 1;

eqonah.iftah('data/locked.eqonah');

//taba3(eqonah.hajm);
//eqonah.hajm(10, 10);

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
			w = 32*eqadd,
			h = 32*eqadd;

		msfoof.nazzaf(hadaf, 0);

//		if (fill)
//		msfoof.maqtoo3(RAEES, w/4, h/4, w/2, h/2);

		var count = 0;
		for (var i in mowdoo3) {
			msfoof.mustateel(RAEES, count*w, 0, w, h, bland, bland);
			eqonah.hajm(count*w, 0);
			eqonah.sattar(hadaf, mowdoo3[i]);
			count++;
		}

//		msfoof.maqtoo3(RAEES, 0, 0, 0, 0);

//		var mjdh = polygon.axav(mowjoodah);
//		if (mjdh) {
//			msfoof.daairah(RAEES,  mjdh.x*cw3,     mjdh.y*cw3   , cw*2, 0, 360, bright, -1);
//			msfoof.daairah(RAEES, (mjdh.x*cw3)+1, (mjdh.y*cw3)+1, cw*2, 0, 360, bland, -1);
//		}
//		for (var n = 0; n < total; n++) {
//			mjdh = polygon.axav(n);
//			msfoof.daairah(RAEES,   mjdh.x*cw3, 0+mjdh.y*cw3, cw, 0, 360, bland, -1);
//			msfoof.daairah(RAEES, 1+mjdh.x*cw3, 1+mjdh.y*cw3, cw, 0, 360, bright, -1);
//		}

		return 1;
	}
	return 0;
}
function b_lowhah (wm) {
	if (wm.haal) {
		var y = 0;
		if (wm.shift) {
//			if (wm.ism == 's') layyin = !layyin, msfoof.layyin(RAEES, layyin), y = 1;
//			if (wm.ism == 'j') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)-.25 ), y = 1;
//			if (wm.ism == 'l') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)+.25 ), y = 1;
			if (wm.ism == 'j') {
				eqadd -= .25;
				eqonah.qadd(eqadd);
				y = 1;
			}
			if (wm.ism == 'l') {
				eqadd += .25;
				eqonah.qadd(eqadd);
				y = 1;
			}
			if (['j', 'l'].indexOf(wm.ism) > -1) {
				if (eqadd < .25)	eqadd = .25;
				if (eqadd > 5  )	eqadd = 5;
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
