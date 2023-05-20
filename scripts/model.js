require(Nizaam.scripts_path+'scripts/mtl-parser.js');
require(Nizaam.scripts_path+'scripts/mtl-builder.js');
require(Nizaam.scripts_path+'scripts/obj-parser.js');
var Model, MODEL = scene.type('model');
;(function(){
var prefix = 'models/', default_transform = mat4.create(), model_uid = 0;
function calc_bounding_box(model) {
	var children = Object.keys(model.children);
	if (children.length) {
		var bounds = BoundingBox();
		var min = vec3.create();
		var max = vec3.create();
		bounds = model.children[ children[0] ].bbox;

		for (var i in model.children) {
			var child = model.children[i];
			if (child.model) {
				var tempBounds = child.bbox;

				min[0] = (bounds.min[0] < tempBounds.min[0]) ? bounds.min[0] : tempBounds.min[0];
				min[1] = (bounds.min[1] < tempBounds.min[1]) ? bounds.min[1] : tempBounds.min[1];
				min[2] = (bounds.min[2] < tempBounds.min[2]) ? bounds.min[2] : tempBounds.min[2];
				bounds.min = min;

				max[0] = (bounds.max[0] > tempBounds.max[0])? bounds.max[0] : tempBounds.max[0];
				max[1] = (bounds.max[1] > tempBounds.max[1])? bounds.max[1] : tempBounds.max[1];
				max[2] = (bounds.max[2] > tempBounds.max[2])? bounds.max[2] : tempBounds.max[2];
				bounds.max = max;
			}
		}
		model.wireframe.bbox = model.bbox = bounds;
	}
}
function calc_bbox_visibility(yes) {
	var o = this;
	if (isundef(yes)) yes = scene.includes( o.guid );
	for (var i in o.children) {
		var child = o.children[i];
		if (child.model && scene.includes(i)) {
			yes = 1;
			break;
		}
	}

	if (o.wireframe) o.wireframe.hidden = !yes;
}
function load_model(o) {
	var queue = Queue();

	queue.set(function (done, queue, args) { // read obj + add model placeholder
		args.placeholder = o;
		var path = args.path+args.name+'.obj';
		args.log = function (info) {
			args.placeholder.info = ''+info; // log(info);
			composer.damage();
		};
		args.log('file\treading '+path);
		Filesystem.read(path, function (obj_data) {
			if (!obj_data) {
				queue.finish(Error('not found'));
				scene.remove(args.placeholder.guid);
			}
			args.obj_data = obj_data;
			done(queue, args);
		});
	});
	queue.set(function (done, queue, args) { // parse obj + add meshes
		if (!args.obj_data) { done(queue, args); return; }
		args.log('file\tdecoding obj binary to text');
		asio.thread(args, parse_obj, function (obj_parsed) {
			if (iserr(obj_parsed) || isundef(obj_parsed)) { pretty(obj_parsed); return; }

			delete args.obj_data; // free memory

//			log('  libraries :',  obj_parsed.libraries			   );
//			log('  materials :',  obj_parsed.matnames	.length    );

			for (var i in obj_parsed.meshes) {
				var mesh_info = obj_parsed.meshes[i];
//				log('  name  :',  mesh_info.name);
//				log('    vertices  :',  mesh_info.vertices  	.length / 3);
//				if (mesh_info.faces)
//				log('    faces     :',  mesh_info.faces			.length / 3);
//				log('    texcoords :',  mesh_info.texcoords		.length / 2);
//				log('    texindices:',  mesh_info.texindices	.length    );
//				log('    normals   :',  mesh_info.normals		.length / 3);

				var mesh = Mesh( mesh_info.name );
				mesh.model = 1; // owned by a model, hint for mats

				mesh.on_select = function () {
					if (args.placeholder.allow_child_selection)
						args.placeholder.calc_bbox_visibility(1);
					else {
						scene.select(args.placeholder.guid);
						return 1;
					}
				};
				mesh.on_deselect = function () { args.placeholder.calc_bbox_visibility(0); };

				scene.reparent(args.placeholder, mesh);
				var mb = mesh.mb;
				
				mb.tris  = Batch(mesh_info.faces	);
				mb.verts = Batch(mesh_info.vertices );
				mb.uv    = Batch(mesh_info.texcoords);
				mb.norms = Batch(mesh_info.normals	);
				
				mb.upload();
				mesh.calc_bounding_box();
			}
			
			calc_bounding_box( args.placeholder );
			args.placeholder.wireframe.add_bbox_wireframe();

			args.obj_parsed = obj_parsed;
			done(queue, args);
		});
	});
	queue.set(function (done, queue, args) { // read mtl libs (fornow limit = 1)
		var obj_parsed = args.obj_parsed, libs = obj_parsed.libraries;
		if (libs && libs.length) {
			args.log('libraries\treading '+args.path+libs[0]);
			Filesystem.read(args.path+libs[0], function (mtl_data) {
				args.mtl_data = mtl_data;
				done(queue, args);
			});
		} else done(queue, args);
	});
	queue.set(function (done, queue, args) { // parse mtl
		if (args.mtl_data) {
			args.log('mtl\tdecoding');
			asio.thread({
				path: args.path,
				mtl_data: args.mtl_data,
			}, parse_mtl, function (materials) {
				delete args.mtl_data;
				args.materials = materials;
				done(queue, args);
			});
		} else done(queue, args)
	});
	queue.set(function (done, queue, args) { // fetch material assets
		if (args.materials) {
			args.log('materials\tbuilding');
			build_material(args, function () {
				done(queue, args);
			});
		} else done(queue, args)
	});
	queue.run(function (queue, args) { // apply material assets
		if (iserr(args) || isundef(args)) { pretty(args); return; }

		var materials = args.materials;
		var model = args.model;
		
//		pretty(model, materials);

		args.log('materials\tuploading');

		Duktape.compact(global); // reduce memory footprint

		if (materials) {
			for (var i in materials) {
				var mat = materials[i];
				if (mat.diffuse_image) {
					for (var i in args.placeholder.children) {
						var child = args.placeholder.children[i];
						if (child.model) {
							child.mb.set_image(mat.diffuse_image);
							child.mb.upload();
						}
					}
					mat.diffuse_image = undefined; // gc later
				}
			}
		}

		composer.damage();
		args.log('materials\tdone');
		if ( isfun(args.placeholder.on_ready) ) {
			args.placeholder.on_ready();
		}

		set_timeout(3000, function () {
			delete args.placeholder.info;
			composer.damage();
		});
	}, {
		path: prefix,
		name: o.name,
	});
}
Model = function (name, loc, rot, sca) {
	var o = {
		name     : name || ++model_uid	,
		location : v3(0)		,
		rotation : v3(0)		,
		scale	 : v3(1,1,1)	,
		info	 : '',
		bbox	 : BoundingBox(),
		allow_child_selection: 0,
		calc_bbox_visibility: calc_bbox_visibility,
	};
	if (loc) o.location = v3(loc.x, loc.y, loc.z);
	if (rot) o.rotation = v3(rot.x, rot.y, rot.z);
	if (sca) o.scale	= v3(sca.x, sca.y, sca.z);
	o = scene.add(o.name, MODEL, o);

	var wires = Mesh( o.name+'_wires' );
	wires.hidden = 1;
	o.wireframe = wires;
	scene.reparent(o, wires);

	o.on_select = function () { o.calc_bbox_visibility(1); };
	o.on_deselect = function () { o.calc_bbox_visibility(0); };

	load_model(o);
	return o;
}
hooks.set('scene.info', function (o) { if (o.type == MODEL) {
	var str = '', count = 0;
	var l = o.get('location');
	var r = o.get('rotation');
	var s = o.get('scale');

	str += 'loc x'+l.x+' y'+l.y+' z'+l.z
		+'\nrot x'+r.x+' y'+r.y+' z'+r.z
		+'\nsca x'+s.x+' y'+s.y+' z'+s.z;

	for (var i in o.children) {
		if (o.children[i].model) count++;
	}
	if (count) str += '\n'+(count+' children');
	if (o.bbox) {
		var min = v3.precision_float_array(o.bbox.min);
		var max = v3.precision_float_array(o.bbox.max);
		str += '\nbounds';
		str += '\n  min '+vec3.str(min);
		str += '\n  max '+vec3.str(max);
	}
	if (isarr(o.key_frames) && o.key_frames.length) {
		str += '\nkey frames '+o.key_frames.length;
	}
	return str;
} });
hooks.set('scene.keyboard', function (o, k, m, held) { if (o.type == MODEL) {
	if (isfun(o.on_keyboard)) return o.on_keyboard(k, m, held);
} });
hooks.set('scene.pointer', function (o, m) { if (o.type == MODEL) {
	if (isfun(o.on_pointer)) return o.on_pointer(m);
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == MODEL) {
	return animate_key_frames(o);
} });
hooks.set('scene.move', function (o, transform) { if (o.type == MODEL) {
	o.location.transform = v3.add(o.posed('location'), transform);
	if (isfun(o.on_move)) o.on_move(o.location.transform, transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == MODEL) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == MODEL) {
	o.scale.transform = v3.add(o.posed('scale'), transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == MODEL) {
	if (o.location.transform) {
		if (isfun(o.on_move)) o.on_move(o.location.transform);
		o.location.pose = o.location.transform;
		o.location.transform = 0;
	}
	if (o.rotation.transform) {
		o.rotation.pose = o.rotation.transform;
		o.rotation.transform = 0;
	}
	if (o.scale.transform) {
		o.scale.pose = o.scale.transform;
		o.scale.transform = 0;
	}
} });
hooks.set('scene.cancel', function (o) { if (o.type == MODEL) {
	if (o.location.transform) {
		if (isfun(o.on_move)) o.on_move(o.location.transform);
	}
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.scale.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == MODEL) {
	if (o.location.pose) {
		if (isfun(o.on_move)) o.on_move(o.location);
	}
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.scale.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == MODEL) {
	if (o.mb) o.mb.remove();
} });

//var box = Mesh();
//box.add_cube(10);
//box.select(box.recent.front).apply_color(c_red);
//box.select(box.recent.top).apply_color(c_green);
//box.select(box.recent.bottom).apply_color(c_blue);
//box.select(box.recent.back).apply_color(c_yellow);
//box.calc_bounding_box();
//
//var wires = Mesh( 'wires' );
//wires.bbox = box.bbox;
//pretty( wires.bbox );
//wires.add_bbox_wireframe();

})();
