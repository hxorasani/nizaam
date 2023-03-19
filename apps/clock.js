//taba3("---badaa---");
var count = 0, init = 0, gy = 0, target = 0,
	barsize, barsize2, counter = '',
	duration = 3, xatqadd = 4.8;
var strokew = 1.5, appbg = 0x222222, appbg2 = 0x333333,
	marksmall = 0xaaaaaa, markbig = 0x666666;
var m = {h:1040};

//kaatib.iftah("./data/test2.matn");
kaatib.muashshar_zaahir(0);
kaatib.mutaharrikah(1);
kaatib.qadd(2);
kaatib.mu3aaq(1);
kaatib.taht(0);

function now() {
	return new Date().getTime();
}
function b_hajm (hadaf) { // on resize
	if (hadaf == MUSHEER) {
		barsize = musheer.h/4;
		barsize2 = barsize*2;
	}
	if (hadaf == RAEES) {
//		msfoof.layyin(hadaf, 0);
		kaatib.hajm(raees.w/4, 10, raees.w/2, raees.h);
	}
}
function drawcircle(hadaf, ww, hh, w2, h2, r) {
	var dd = new Date(), sw = hadaf == MUSHEER ? strokew/4 : strokew;
	var seconds = dd.getSeconds();
	var secs = (seconds+(dd.getMilliseconds()/1000) )/60;
	var minutes = dd.getMinutes();
	var mins = (dd.getMinutes()+secs)/60, ox = ww*.25;
	var hours = dd.getHours();
	if (hours > 12) hours -= 12;
	var hour = (hours+mins)/12, q1, q2;
	var q3 = riyaadah.zin(w2, h2, -r+(r*.8), 90+360*secs);
	var q4 = riyaadah.zin(w2, h2, r, 90+360*secs);
	var q5 = riyaadah.zin(w2, h2, r*.75, 90+360*mins);
	var q6 = riyaadah.zin(w2, h2, r*.5, 90+360*hour),
		c1 = alwaan.hsl2rgb(secs, 1, .8);

	if (hadaf == MUSHEER) {
		msfoof.nazzaf(hadaf, 0);
		msfoof.daairah(hadaf, w2, h2, r*.9, 0, 360, -1, 0x222222);
	}
	
	if (hadaf == RAEES) {
		var oy = h2+r+20, oy2 = h2+r+20-4;
		
		msfoof.mustateel(hadaf, 0, 0 , ww, h2, -1, alwaan.hsl2rgb(secs, 1, .04));
		msfoof.mustateel(hadaf, 0, h2, ww, h2, -1, alwaan.hsl2rgb(secs, 1, .03));

//		msfoof.satar(hadaf, ox, oy  , ox+w2, oy, alwaan.hsl2rgb(secs, 1, .25), -1);
//		msfoof.mustateel(hadaf, ox, oy, w2*mins, barsize, -1, alwaan.hsl2rgb(secs, 1, .5));
//
//		msfoof.satar(hadaf, ox, oy2-barsize, ox+w2, oy2-barsize, alwaan.hsl2rgb(secs, 1, .25), -1);
//		msfoof.mustateel(hadaf, ox, oy2-barsize, w2*hour, barsize, -1, alwaan.hsl2rgb(secs, 1, .4));
		
		msfoof.qadd(hadaf, sw+2);
//		msfoof.daairah(hadaf, w2, h2-2, r*.88, 0, 180, mowdoo3.waahid, -1);
//		msfoof.daairah(hadaf, w2, h2+4, r*.88, 180, 180, 0, -1);

		kaatib.hajm(raees.w/4, 10, raees.w/2, raees.h);
		kaatib.tasteer(hadaf, hours+':'+minutes+':'+seconds);
		kaatib.idhan(hadaf, 1);

		kaatib.hajm(raees.w/4, raees.h-60, raees.w/2, raees.h);
		kaatib.tasteer(hadaf, dd.getDate()+', '+(dd.getMonth()+1)+', '+dd.getFullYear());
		kaatib.idhan(hadaf, 1);
	}
	msfoof.qadd(hadaf, sw);
	
//	msfoof.satar(hadaf, w2+1, h2+1, q5.x+1, q5.y+1, 0);
//	msfoof.satar(hadaf, w2+1, h2+1, q6.x+1, q6.y+1, 0);
	
	var maj = 0;
	for (var i = 0; i < 360; i+=6) {
		maj = i % 30 == 0;
		if ( (hadaf == MUSHEER && maj) || (hadaf == RAEES) ) {
			q1 = riyaadah.zin(w2, h2, r*(maj?.8:.85), 90+i);
			q2 = riyaadah.zin(w2, h2, r*.9, 90+i);
			msfoof.satar(hadaf, q1.x, q1.y, q2.x, q2.y, maj ? mowdoo3.waahid : mowdoo3.acnaan);
		}
	}

	msfoof.satar(hadaf, w2, h2, q5.x, q5.y, mowdoo3.qaadim);
	msfoof.satar(hadaf, w2, h2, q6.x, q6.y, mowdoo3.qaadim);

	msfoof.qadd(hadaf, sw+.25);
	msfoof.satar(hadaf, w2+1, h2+1, q3.x+1, q3.y+1, 0); // tail red
	msfoof.satar(hadaf, w2+1, h2+1, q4.x+1, q4.y+1, 0); // secs red

	if (hadaf == RAEES)
	msfoof.daairah(hadaf, w2, h2, r*.04, 0, 360, 0, c1);

	msfoof.qadd(hadaf, sw);

	msfoof.satar(hadaf, w2, h2, q3.x, q3.y, c1); // tail red
	msfoof.satar(hadaf, w2, h2, q4.x, q4.y, c1); // secs red
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES) {
//		msfoof.nazzaf(hadaf, appbg);
		drawcircle(hadaf, raees.w, raees.h, raees.w/2, raees.h/2, musheer.w*xatqadd);
//		if (target !== gy) {
			return 2;
//		}
//		return 1;
	}
	if (hadaf == MUSHEER) {
		var w2 = musheer.w/2, h2 = musheer.h/2;
		drawcircle(hadaf, musheer.w, musheer.h, w2, h2, w2-2);
//		if (target !== gy) {
			return 2;
//		}
//		return 1;
	}
	return 0;
}
function b_lowhah (wm) {
	if (wm.haal) {
//		return 0;
//		var muntahaa = kaatib.muntahaa();
		var muntahaa = m;
		var max, step = 24, y = 0;
		max = muntahaa.h > 0 ? muntahaa.h-raees.h : raees.h;

		if (wm.shift) step = 5;
		
		if (wm.ism == 'u') xatqadd-=.05, y = 1;
		if (wm.ism == 'i') xatqadd+=.05, y = 1;
		if (wm.ism == 's') {
			var layyin;
			if (wm.shift) {
				layyin = msfoof.layyin(MUSHEER, -1)
				msfoof.layyin(MUSHEER, layyin?0:1), y = 1;
			} else {
				layyin = msfoof.layyin(RAEES, -1)
				msfoof.layyin(RAEES, layyin?0:1), y = 1;
			}
		}

		if (wm.ism == 'z' && strokew > .25)
			strokew-=.25, y = 1;
		if (wm.ism == 'x' && strokew < 5)
			strokew+=.25, y = 1;

		if (target > 0 && wm.ism == 'j') target -= step, y = 1;
		else if (target <= max && wm.ism == 'k') target += step, y = 1;
		
		if (muntahaa.h < raees.h) target = 0;
		else if (target > max) target = max;
		else if (target < 0) target = 0;
		
		if (y) {
//			taba3('b_lowhah');
			return 1;
		}
	}
	return 0;
}