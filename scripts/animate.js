/* TODO
 * if root prop is an array, apply animation to all sub-props like:
 * call once, apply thrice
 * scale: [arr] -> scale x y z
 * */
require(Nizaam.scripts_path+'scripts/core/ease.js');
function is_keyframe(o) {
	return !isundef(o._kf);
}
function animate_key_frames(o) {
	var frames = 0, a = o.animate; // TODO .animate into a function that pushes keyframes!
	if (isarr(o.key_frames) && o.key_frames.length) {
		a = o.key_frames[0]; // get the first keyframe
	}

	if (a) {
		animatables.forEach(function (prop) {
			var remove = 1;
			if (a[prop]) {
				if (is_keyframe(a[prop])) {
					frames += animate(a, prop, o);
					remove = 0;
				} else {
					for (var i in a[prop]) {
						if (a[prop][i]) {
							frames += animate(a[prop], i, o[prop]);
							remove = 0;
						} else {
							delete a[prop][i];
						}
					}
				}
				if (!remove && isfun(o.on_move) && prop == 'location') {
					o.on_move(o[prop]);
				}
				if (remove) delete a[prop];
			}
		});
		if (isundef(a) || Object.keys(a).length == 0) { // keyframe processed, pop it
			if (isarr(o.key_frames)) {
				o.key_frames.shift(0); // remove the first keyframe
				if (o.key_frames.length) // let another frame be processed
					frames++;
			}
		}
	}
	return frames;
}
function Keyframe(start, end, duration, ease) { // no holdoff
	return {
		_kf: 1,
		s: start,
		e: end,
		d: duration,
		t: ease || 'sinein'
	};
}
function KeyframeXYZ(start, end, duration, ease) {
	return {
		x: Keyframe(start, end, duration, ease),
		y: Keyframe(start, end, duration, ease),
		z: Keyframe(start, end, duration, ease)
	};
}
function KeyframeRGBA(start, end, duration, ease) {
	return {
		r: Keyframe(start, isarr(end) ? end[0] : end, duration, ease),
		g: Keyframe(start, isarr(end) ? end[1] : end, duration, ease),
		b: Keyframe(start, isarr(end) ? end[2] : end, duration, ease),
		a: Keyframe(start, isarr(end) ? end[3] : end, duration, ease)
	};
}
function animate(anim, prop, obj) {
	var now = new Date().getTime(),
		a = anim[prop],
		hol = a.h || 0, // hold off, delay
		sta = isundef(a.s) ? a.s = obj[prop] : a.s,
		end = a.e,
		dur = a.d,
		eas = a.t || 'sinein', // ease type
		fun = a.f;
		
	if (isundef(a._t)) a._t = now;
	if (isundef(a._d)) {
		a._d = dur + now + hol;
		obj[prop] = sta;
	}
	if (!hol) a._h = 1;

	a.n = now - a._t; // now
	if (isundef(a._h) && a.n > hol) {
		a._h = 1; // hold off passed
		a._t = now; // reset now
		a.n = 0;
	}

//	if (prop == 'z') log(a.n/(dur+hol)+', '+dur+'    '+obj[prop]+', '+end);
	if (a._h) {
		// curX = EaseSineIn(curTime, startX, finalX - startX, duration);
		var pct = a.n/(dur+hol);
		if (ease[eas]) obj[prop] = parsefloat(ease[eas](a.n, sta, end - sta, dur), 5);
		if (!ease[eas] || a.n >= dur) {
			obj[prop] = end;
			if (isfun(fun)) fun();
			anim[prop] = undefined;
		}
	}
	return 1;
}
var animatables = ['location', 'rotation', 'scale', 'color', 'visible'];
