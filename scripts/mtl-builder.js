function build_material(args, done) {
	args.log('materials\tbuilding');

	var keys = Object.keys(args.materials);
	var first = args.materials[keys[0]];
	if (first && first.diffuse_map) {
		args.log('diffuse\treading ...'+first.diffuse_map.slice(-30));

		Filesystem.read(first.diffuse_map, function (binary) {
			if (binary instanceof Error || !binary) {
				pretty(Error('not found '+first.diffuse_map));
				done();
			} else
			asio.thread(binary, function (binary) {
//				log('texture\tdecoding binary', binary.length / 1000, 'KB');
				return codec.decode_image(binary);
			}, function (image) {
				if (image && !(image instanceof Error)) {
					args.log('texture\treferenced '+(image.data.length / 1000)+' KB');
				}
				first.diffuse_image = image;
				done();
			});
		});
	} else done();
}