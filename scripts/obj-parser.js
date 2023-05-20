var parse_obj = function (args) {
var path = args.path, obj_string = new TextDecoder().decode( args.obj_data );

//log('file\tparsing obj length', obj_string.length / 1000, 'KB');
// funcs are defd inside to allow for thread dumps to ref them later
var vertices = 0, uvs = 0, triangles = 0, uv_ids = 0, normals = 0,
	matindices = [], mat_names = [], libraries = [], meshes = {}, counts = {},
	normals_count = 0, texcoords_count = 0, face_count = 0;

var vert_re = new RegExp(/[\d\.\-\+]+/gi);
var index_re = new RegExp(/[\d\/]+/gi);

var parsefloat = parseFloat;
var parseint = parseInt;
var lines = obj_string.split('\n');
function starts_with (o, s) { return o.startsWith(s); };

var current_material = 0, mesh = 0, count = 0;
function push_verts_by_index(target_arr, verts, index) {
	target_arr.push( verts[ index*3 + 0 ] );
	target_arr.push( verts[ index*3 + 1 ] );
	target_arr.push( verts[ index*3 + 2 ] );
}
function getMaterialIndex(name) {
	if (mat_names.indexOf(name) > -1) {
		return mat_names[name];
	} else {
		return mat_names.push(name) - 1;
	}
}
function remap_texture(model) { // TODO allocation pass
	var verts = [], texcoords = [], normals = [];

	for (var i = 0; i < model.faces.length; i+=3) {
		var x = model.faces[i+0],
			y = model.faces[i+1],
			z = model.faces[i+2];
		push_verts_by_index(verts, model.vertices, x);
		push_verts_by_index(verts, model.vertices, y);
		push_verts_by_index(verts, model.vertices, z);
		model.faces[i+0] = i+0;
		model.faces[i+1] = i+1;
        model.faces[i+2] = i+2;
	}
//	model.faces = 0;
	
	for (var i = 0; i < model.texindices.length; i++) {
		var t = model.texindices[i];
		var x = model.texcoords[t*2 +0],
			y = model.texcoords[t*2 +1];
		texcoords.push(x, y);
	}
	model.texcoords = texcoords;

	for (var i = 0; i < model.norindices.length; i++) {
		var t = model.norindices[i];
		var x = model.normals[t*3 +0],
			y = model.normals[t*3 +1],
			z = model.normals[t*3 +2];
		normals.push(x, y, z);
	}
	model.normals = normals;

	model.vertices = verts;
}
function parse_lines(allocate_only) {
	normals_count = texcoords_count = face_count = count = 0;
	for (var i = 0; i < lines.length; ++i) {
		var line = lines[i];
		var coords, indices;
		if (starts_with(line, "v ")) {
			coords = line.slice(2).match(vert_re);
			if (!allocate_only) {
				mesh.vertices[ count.vertices + 0 ] = parsefloat(coords[0]);
				mesh.vertices[ count.vertices + 1 ] = parsefloat(coords[1]);
				mesh.vertices[ count.vertices + 2 ] = parsefloat(coords[2]);
			}
			count.vertices += 3;
		} else if (starts_with(line, "vn ")) {
			coords = line.slice(2).match(vert_re);
			if (!allocate_only) {
				mesh.normals[ count.normals + 0 ] = parsefloat(coords[0]);
				mesh.normals[ count.normals + 1 ] = parsefloat(coords[1]);
				mesh.normals[ count.normals + 2 ] = parsefloat(coords[2]);
			}
			count.normals += 3;
		} else if (starts_with(line, "vt ")) {
			coords = line.slice(3).match(vert_re);
			if (!allocate_only) {
				mesh.texcoords[ count.texcoords + 0 ] =     parsefloat(coords[0]);
				mesh.texcoords[ count.texcoords + 1 ] = 1 - parsefloat(coords[1]);
			}
			count.texcoords += 2;
		} else if (starts_with(line, "f ")) {
			indices = line.slice(2).match(index_re);

			if (indices.length != 3) throw Error("model not triangulated!");

			var t = [], u = [], components = [];
			for (var j in indices) {
				components = indices[j].split("/");
				if (!allocate_only) mesh.faces[ count.faces ] = parseint(components[0]) - 1 - face_count;
				count.faces += 1;

				if (!allocate_only) mesh.texindices[ count.texindices ] = parseint(components[1]) - 1 - texcoords_count;
				count.texindices += 1;

				if (!allocate_only) mesh.norindices[ count.norindices ] = parseint(components[2]) - 1 - normals_count;
				count.norindices += 1;
			}
			
			matindices.push(current_material);
		} else if (starts_with(line, "o ")) {
			var name = line.slice(2);
			if (allocate_only) {
				meshes[name] = {
					name      : name,
					matindices: matindices,
				};
			}
			if (count) {
				face_count += count.vertices / 3;
				texcoords_count += count.texcoords / 2;
				normals_count += count.normals / 3;
			}
			counts[name] = { // this is renewed twice to allow reuse
				vertices  : 0,
				faces     : 0,
				normals   : 0,
				norindices: 0,
				texcoords : 0,
				texindices: 0,
			};
			mesh = meshes[name]; // allocate pass should have added this mesh
			count = counts[name];
		} else if (starts_with(line, "mtllib ")) {
			if (!allocate_only) libraries.push( line.slice(7) );
		} else if (starts_with(line, "usemtl ")) {
			if (!allocate_only) current_material = getMaterialIndex(line.slice(7));
		}
	}
	
	if (allocate_only)
	for (var i in counts) {
		meshes[i].vertices	= new Float32Array( counts[i].vertices	);
		meshes[i].faces		= new Uint32Array ( counts[i].faces		);
		meshes[i].norindices= new Uint32Array ( counts[i].norindices);
		meshes[i].texcoords	= new Float32Array( counts[i].texcoords	);
		meshes[i].texindices= new Uint32Array ( counts[i].texindices);
		meshes[i].normals	= new Int8Array   ( counts[i].normals	);
	}
}

//log('buffers\tallocating');
parse_lines(1);
//log('buffers\tfilling');
parse_lines();
//log('buffers\tremapping textures');
for (var i in meshes) {
	remap_texture( meshes[i] );
}

return {
	meshes    : meshes,
	libraries : libraries,
	matnames  : mat_names,
	matindices: matindices,
};
}
