//taba3("---badaa---");
var mu3aaq = 1, ptr, mshrdrwn, path = "/home/nano/Notebooks/Docs/kde.txt";
var rightclick = 0, drag = 0;

kaatib.iftah(path);
kaatib.muashshar_zaahir(1);
kaatib.mutaharrikah(1);
//kaatib.mu3aaq(mu3aaq);

function now() {
	return new Date().getTime();
}
function on_bound_window (r) { // on resize
//	msfoof.layyin(hadaf, 0);
//	kaatib.qadd(16);
	kaatib.hajm(30, 30, r.w-60, r.h-60);
//	canvas.haal(1); // turn canvas off
}
function drawcircle(hadaf, w2, h2, r, pct, m) {
	var w4 = w2-7;
	var q  = riyaadah.zin(w2, h2, r-2, 90+360*pct);
	var q2 = riyaadah.zin(w2, h2, r-6, 90+360*pct);
	msfoof.qadd(hadaf, 1);

	msfoof.daairah(hadaf, w2, h2, r-10*pct-4, 99+(360*pct), 340-(360*pct), -1, 0x777777);
	msfoof.daairah(hadaf, w2, h2, r-5, 90, 360*pct, 0xaaaaaa, -1);
	msfoof.daairah(hadaf, w2, h2, r-8, 90, 360*pct, -1, 0x999999);
	msfoof.qadd(hadaf, 1);
	msfoof.satar(hadaf, q.x, q.y, q2.x, q2.y, 0x337788, 1);
}
function on_paint_window () { // on draw
	canvas.save();

	var yes = 1;

	canvas.line_width(1);
	canvas.lwn(.1, .3, .1);
	canvas.paint();

	var khjm = kaatib.hajm();
	if (khjm) {
		canvas.rect(khjm.x-1, khjm.y+.5, khjm.w, khjm.h);
		canvas.lwn(.3, .6, .5);
		canvas.stroke();
//		canvas.clip();
		canvas.renew_path();
	}
	
//	msfoof.nazzaf(0);
//	canvas.xat("Segoe UI Light");
//	canvas.xat("Segoe UI");
//	canvas.xat("Ubuntu Thin");
//	canvas.xat("Ubuntu Condensed");
//	canvas.xat("Ubuntu Mono");
//	kaatib.qadd(64);
	
	kaatib.ihsab();

	var xh = canvas.font_bounds();

	for (var i = 0; i < kaatib.tool(); ++i) {
		var harf = kaatib.harf(i);
//		taba3(harf.x, harf.y);
		if (harf && harf.zaahir) {
//			canvas.rect(harf.x+.5, harf.y-xh.a+xh.d+.5, harf.w-.5, xh.a+xh.d-1.5);
//			canvas.rect(harf.x+.5, harf.y  -(harf.h)  +.5, harf.w, harf.h+xh.d);
//			canvas.rect(harf.x+.5, harf.y+.5, harf.w, harf.h);
//			canvas.rect(harf.ax+.5, harf.ay+.5, harf.aw, harf.ah);
		}
	}
	canvas.lwn(.1, 1, .1);
	canvas.stroke();
	canvas.renew_path();

	if (ptr) {
		if (rightclick && rightclick.zaahir) {
			canvas.move_to(20, 20);
			canvas.text('rightclick: '+rightclick.x+', '+rightclick.y);
		}

		var harf = kaatib.cu3aa3(ptr.x, ptr.y);
		if (harf && harf.zaahir) {
//			canvas.rect(harf.x+.5, harf.y  -(harf.h)  +.5, harf.w, harf.h+xh.d);
			canvas.rect(harf.x+.5, harf.y+.5, harf.w, harf.h);
			canvas.lwn(1, 0, 1);
			canvas.fill();
			canvas.renew_path();
		}
	}

	canvas.lwn(1, 1, 1);
	kaatib.idhan(1);

	canvas.restore();
	return yes;
}
function on_paint_indicator () {
	var yes;
	if (!mshrdrwn) {
		mshrdrwn = 1;
		canvas.move_to(3, 12);
		canvas.font_size(12);
//		canvas.lwn(0, 0, .1);
		canvas.paint();
		canvas.lwn(1, 1, 1);
		canvas.text(path.substr(-10));
//		msfoof.nazzaf(0);
//		drawcircle(w2, h2, w2, pct, m);
//		if (target !== gy) {
//			return 2;
//		}
		yes = 1;
	}
	
	return yes;
}
function b_icaarah (wm) {
	var yes = 0;
	ptr = wm;
	
	if (!wm.w) { // when not scrolling
		if (wm.haal == -1) {
			drag = 1; yes = 1;
		}
		if (wm.haal == 1) {
			drag = 0; yes = 1;
		}
		if (drag) yes = 1;
		if (wm.n == 2) {
			rightclick = kaatib.cu3aa3(wm.x, wm.y);
			yes = 1;
		}
	}

	return yes;
}
function b_lowhah (wm) {
	if (wm.haal) {
		var k = keyism[wm.key];
		if (k == 'o' && wm.ctrl) {
			kaatib.iftah(path);
			return 1;
		}
		if (k == 's' && wm.ctrl) {
			// disabled for safety
//			kaatib.save("./data/test2.matn");
			return 1;
		}
		if (k == 'm' && wm.ctrl) {
			mu3aaq = !mu3aaq;
			kaatib.mu3aaq(mu3aaq);
			return 1;
		}
	}
	return 0;
}
