var bland = 0x333333, bright = 0xaaaaaa, fill = 1, kunikbar = 1;

function now() {
	return new Date().getTime();
}
function b_hajm (hadaf) { // on resize
	msfoof.layyin(hadaf, 1);
	msfoof.qadd(hadaf, .1);
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES) {
		var cw = 10,
			w = musheer.w*cw,
			h = musheer.h*cw;

//		msfoof.nazzaf(hadaf, 0);

		msfoof.ikbar(MUSHEER, RAEES, cw, cw);

		for (var x = 0; x < w; x+=cw) msfoof.satar(hadaf, x, 0, x, h, mowdoo3.caalic);
		for (var y = 0; y < h; y+=cw) msfoof.satar(hadaf, 0, y, w, y, mowdoo3.caalic);

		return 1;
	}
	if (hadaf == MUSHEER) {
		msfoof.nazzaf(hadaf, 0);
//		msfoof.nazzaf_alif(hadaf, 0);
		roundrect(mowdoo3.waahid);
//		matrix.print_alpha(hadaf, bright);
//		msfoof.nazzaf_alif(hadaf, 0);

		return 1;
	}
	return 0;
}
function b_lowhah (wm) {
	if (wm.haal) {
		var y = 0;
		return y;
	}
	return 0;
}
function roundrect (c) {
	var w = musheer.w/2,
		h = musheer.h/1.2,
		pct = .25,
		r = w*pct, d = r*2,
		wr = w*pct, hr = h*pct,
		wr2 = wr*2, hr2 = hr*2,
		otl = 0xff0000;

	msfoof.mustateel(MUSHEER, wr, 0, d, hr, otl, c);			// TOP
	msfoof.mustateel(MUSHEER, 0, hr, r, hr2, otl, c);			// LEFT
	msfoof.mustateel(MUSHEER, wr, hr, d, h-hr2, otl, c);		// CENTER
	msfoof.mustateel(MUSHEER, w-wr, hr, r+1, hr2, otl, c);		// RIGHT
	msfoof.mustateel(MUSHEER, wr, h-hr, wr2, hr+1, otl, c);		// BOT
	msfoof.daairah  (MUSHEER, wr, hr, r, 0, 90, -1, c);			// TL
//	msfoof.daairah  (MUSHEER, w-wr, hr, r, 90, 90, -1, c);		// TR
//	msfoof.daairah  (MUSHEER, wr, h-hr, r, 270, 90, -1, c);		// BL
//	msfoof.daairah  (MUSHEER, w-wr, h-hr, r, 180, 90, -1, c);	// BR
}



