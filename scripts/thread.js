require('apps/core.js');
;(function(){
var prefix = '/home/nano/Projects/raylib/examples/models/resources/models/obj/';
//var prefix = '/home/nano/Pictures/Wallpapers/';
Filesystem.dirs(prefix, function (dirs) {
	var count = 0, max = 8, arr = [], cursor = 0,
		min_x = 0, row = 0, max_h = 0, processing = {};
	function load_image() {
		var o = dirs.pop();
		if (o && count < max) {
			if (o.type == 'file' && (o.name.endsWith('.jpg') || o.name.endsWith('.png'))) {
				var path = prefix+o.name;
				Filesystem.read(path, function (d) {
					if (d && count < max) {
						processing[o.name] = asio.thread({
							name: o.name,
							image: d,
						}, function (d) {
							var name = d.name;
							log('decoding...', name);
							d = codec.decode_image(d.image);
							d.name = name;
							log('done', name);
							return d;
						}, function (d) {
							delete processing[o.name];
							if (d instanceof Error) {
//								log('error', o.name);
								return;
							}
							if (d && count < max) {
								log('loading...', o.name);
								if (cursor == 3) {
									cursor = 0;
									min_x = 0;
									row -= max_h;
								}
	
								var photo = Mesh();
								photo.add_plane();
								arr.push(photo);
								photo.name = path;
								photo.aspect = d.w/d.h;
								var bb = {
									x: -1 * photo.aspect,
									y:  1,
									w:  1 * photo.aspect,
									h: -1,
								};
								min_x -= bb.x-bb.w;
								max_h = Math.max(max_h, -bb.h+bb.y);
								photo.bbox = bb;
								photo.select(photo.recent.tl).set_vert(bb.x, bb.y, 0);
								photo.select(photo.recent.bl).set_vert(bb.x, bb.h, 0);
								photo.select(photo.recent.tr).set_vert(bb.w, bb.y, 0);
								photo.select(photo.recent.br).set_vert(bb.w, bb.h, 0);
								photo.set_image(d);
								var ox = min_x, oy = row;
								photo.location.x = ox;
								photo.location.y = oy;
								cursor++;
								composer.damage();
								count++;
								load_image();
							} else {
								log('cancelling...');
								for (var i in processing) {
									try {
										asio.cancel(processing[i].req);
									} catch (e) {
										log( e );
									}
								}
								for (var i in processing) {
									delete processing[i];
								}
							}
						});
					} else {
//						pretty( 'cannot open', o.name );
					}
				});
			} else {
				load_image();
			}
		}
	}
	load_image();
});
// test data types through threads
asio.thread({
	obj: {
		stuff: 'more',
		things: {
			my: 'fav',
			how: 'way'
		}
	},
	num: 1234.4321,
	und: undefined,
	nul: null,
	tru: true,
	fal: false,
	str: 'string',
	ui8: new Uint8Array([0,1,2,3,4]),
	in8: new Int8Array([0,1,2,3,4]),
	u16: new Uint16Array([0,1,2,3,4]),
	f32: new Float32Array([0,1,2,3,4]),
	fun: function () {
		
	},
	arr: [0,1,2,3,4,5,6,7,8,9,10],
}, function (image) {
	return image;
}, function (data) {
	pretty(data);
});


})();
