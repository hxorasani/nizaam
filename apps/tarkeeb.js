taba3("---badaa---");
var nuqaat = [[15, 1], [36, 1], [24, 47], [4, 47]], mowjoodah = 0,
	bland = 0x333333, bright = 0xaaaaaa, fill = 1, kunikbar = 1;

/* TODO
 * kaatib.axav, allocs a new kaatib and returns its uid, or -1
 * 
 * 
 * 
 * 
 * 
 * 
 * */

for (var n = 0; n < nuqaat.length; n++) {
	nuqaat[n][0] *= 3;
	nuqaat[n][1] *= 3;
}

function now() {
	return new Date().getTime();
}
function b_hajm (hadaf) { // on resize
	msfoof.layyin(hadaf, 0);
}
function b_dihaan (hadaf) { // on draw
	if (hadaf == RAEES && kunikbar) {
		var cw = 10,
			cw3 = cw/3,
			w = raees.w,
			h = raees.h;

		msfoof.nazzaf(hadaf, 0);

		for (var x = 0; x < w; x+=cw) msfoof.satar(hadaf, x, 100  , x, h    , bland);
		for (var y = 0; y < h; y+=cw) msfoof.satar(hadaf, 0, 100+y, w, 100+y, bland);

		msfoof.ikbar_alif(MUSHEER, RAEES, bright);

		var mjdh = nuqaat[mowjoodah];
		if (mjdh) {
			msfoof.daairah(RAEES,  mjdh[0]*cw3,     100+mjdh[1]*cw3, cw*2, bright, -1);
			msfoof.daairah(RAEES, (mjdh[0]*cw3)+1, (100+mjdh[1]*cw3)+1, cw*2, bland, -1);
		}
		for (var n = 0; n < nuqaat.length; n++) {
			msfoof.daairah(RAEES, nuqaat[n][0]*cw3, 100+nuqaat[n][1]*cw3, cw, bland, -1);
			msfoof.daairah(RAEES, 1+nuqaat[n][0]*cw3, 101+nuqaat[n][1]*cw3, cw, bright, -1);
		}

		return 1;
	}
	if (hadaf == MUSHEER) {
		var time = now();
		
		msfoof.nazzaf(hadaf, 0);
		msfoof.nazzaf_alif(hadaf, 0);
		if (fill) {
			poly();
			matrix.print_alpha(hadaf, bright);
//			msfoof.nazzaf_alif(hadaf, 0);
		}
//		for (var n = 1; n < nuqaat.length; n++) {
//			msfoof.satar_alif(hadaf, nuqaat[n][0]/3, nuqaat[n][1]/3, nuqaat[n-1][0]/3, nuqaat[n-1][1]/3, 1, 0);
//		}
//		matrix.print_alpha(hadaf, bright);

		taba3(now()-time);
		return 1;
	}
	return 0;
}
function b_lowhah (wm) {
	if (wm.haal) {
		var y = 0;
		if (wm.shift) {
			if (wm.ism == 'j') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)-.25 ), y = 1;
			if (wm.ism == 'l') msfoof.qadd( MUSHEER, msfoof.qadd(MUSHEER)+.25 ), y = 1;
		} else if (wm.alt) {
			var mjdh = nuqaat[mowjoodah];
			if (wm.ism == 'j') mjdh[0] -= 1, y = 1;
			if (wm.ism == 'l') mjdh[0] += 1, y = 1;
			if (wm.ism == 'i') mjdh[1] -= 1, y = 1;
			if (wm.ism == 'k') mjdh[1] += 1, y = 1;
		} else {
			var mjdh = nuqaat[mowjoodah];
			if (wm.ism == 'a') {
				var a = nuqaat[nuqaat.length-1];
				nuqaat[nuqaat.length] = [a[0], a[1]];
				y = 1;
			}
			if (wm.ism == 'r') {
				nuqaat.pop();
				if (mowjoodah > nuqaat.length-1) mowjoodah = nuqaat.length-1;
				y = 1;
			}

			if (wm.ism == 'f') fill = !fill, y = 1;
			if (wm.ism == 'd') kunikbar = !kunikbar, y = 1;

			if (wm.ism == 'j') mjdh[0] -= 3, y = 1;
			if (wm.ism == 'l') mjdh[0] += 3, y = 1;
			if (wm.ism == 'i') mjdh[1] -= 3, y = 1;
			if (wm.ism == 'k') mjdh[1] += 3, y = 1;

			if (wm.ism == 'z') {
				mowjoodah--;
				if (mowjoodah < 0) mowjoodah = 0;
				y = 1;
			}
			if (wm.ism == 'x') {
				mowjoodah++;
				if (mowjoodah > nuqaat.length-1) mowjoodah = nuqaat.length-1;
				y = 1;
			}
		}
		return y;
	}
	return 0;
}
function poly () {
	var length = nuqaat.length, count = 0;
	if (length) {
		var minX = nuqaat[0][0], maxX = nuqaat[0][0],
			minY = nuqaat[0][1], maxY = nuqaat[0][1];

		for (var n = 1; n < length; n++) {
			minX = Math.min(nuqaat[n][0], minX);
			maxX = Math.max(nuqaat[n][0], maxX);
			minY = Math.min(nuqaat[n][1], minY);
			maxY = Math.max(nuqaat[n][1], maxY);
		}
		
		for (var x = minX; x < maxX; ++x) { // each vert line
			for (var y = minY; y < maxY; ++y) { // from minY to maxY
				// continue if x, y are inside the bounding box
				if (x < minX || x > maxX || y < minY || y > maxY) return 0;

				/*var i = 0, // 
					j = length - 1; // total nuqaat
				for (; i < length; j = i++) { // for each nuqtah
					var ix = nuqaat[i][0], iy = nuqaat[i][1], // current nuqtah
						jx = nuqaat[j][0], jy = nuqaat[j][1]; // last nuqtah
					if (
							(iy > y) != (jy > y) // only one y is greater than cur y
						&&	(
								x < (jx - ix) // cur x is smaller than ...
								*	(y - iy)
								/	(jy - iy) + ix
						)
					) {
						isinside = !isinside;
						if (isinside) {
							msfoof.ja3al_alif(MUSHEER, x, y, 1);
							count++;
						}
					}
				}*/
				var j = length - 1
				var c = 0;
				for (var i = 0; i < length; ++i) {
					if ( (x == nuqaat[i][0]) && (y == nuqaat[i][1]) ) {
						// point is a corner
						c = 1;
					} else
					if ( (nuqaat[i][1] > y) != (nuqaat[j][1] > y) ) {
						var slope = (x-nuqaat[i][0])*(nuqaat[j][1]-nuqaat[i][1])-(nuqaat[j][0]-nuqaat[i][0])*(y-nuqaat[i][1]);
						if (slope == 0) {
							// point is on boundary
							c = 1;
						} else
						if ( (slope < 0) != (nuqaat[j][1] < nuqaat[i][1]) ) {
							c = !c;
						}
					}

					j = i;
				}
				if (c) msfoof.ja3al_alif(MUSHEER, x, y, 1), count++;
			}
		}
	}
//	taba3(count);
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
