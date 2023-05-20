var parse_mtl = function (args) {
//log('file\tdecoding mtl binary to text');
var path = args.path, mtl_string = new TextDecoder().decode( args.mtl_data );

//log('file\tparsing mtl length', mtl_string.length / 1000, 'KB');
// funcs are defd inside to allow for thread dumps to ref them later
var materials = {};

var vert_re = new RegExp(/[\d\.\-\+]+/gi);
var index_re = new RegExp(/[\d\/]+/gi);

var parsefloat = parseFloat;
var parseint = parseInt;
function parsefloat3(str) { return str.match(vert_re).map(parsefloat); }
function starts_with(o, s) { return o.startsWith(s); };
var lines = mtl_string.split('\n');

mat = 0;

function parse_lines() {
	for (var i = 0; i < lines.length; ++i) {
		var line = lines[i];
			 if (starts_with(line, "map_Ka "	))	{	if (mat) mat.  ambient_map	= line.slice(7); }
		else if (starts_with(line, "map_Kd "	))	{	if (mat) mat.  diffuse_map	= line.slice(7); }
		else if (starts_with(line, "map_Ks "	))	{	if (mat) mat. specular_map	= line.slice(7); }
		else if (starts_with(line, "map_Ns "	))	{	if (mat) mat.highlight_map	= line.slice(7); }
		else if (starts_with(line, "bump "		))	{	if (mat) mat.	  bump_map	= line.slice(5); }
		else if (starts_with(line, "map_bump "	))	{	if (mat) mat.	  bump_map	= line.slice(9); }
		else if (starts_with(line, "disp "		))	{	if (mat) mat.displace		= line.slice(5); }
		else if (starts_with(line, "map_d "		))	{	if (mat) mat.alpha			= line.slice(6); }
		else if (starts_with(line, "illum "		))	{	if (mat) mat.illum			= parseint   ( line.slice(6) );		}
		else if (starts_with(line, "d "			))	{	if (mat) mat.dissolve		= parsefloat ( line.slice(2) );		}
		else if (starts_with(line, "Tr "		))	{	if (mat) mat.dissolve		= 1 - parsefloat( line.slice(3) );	}
		else if (starts_with(line, "Ns "		))	{	if (mat) mat.shininess		= parsefloat ( line.slice(3) );		}
		else if (starts_with(line, "Ke "		))	{	if (mat) mat.emission		= parsefloat3( line.slice(3) );		}
		else if (starts_with(line, "Ni "		))	{	if (mat) mat.ior			= parsefloat ( line.slice(3) );		}
		else if (starts_with(line, "Kt "		))	{	if (mat) mat.transmittance	= parsefloat3( line.slice(3) );		}
		else if (starts_with(line, "Ks "		))	{	if (mat) mat.specular		= parsefloat3( line.slice(3) );		}
		else if (starts_with(line, "Kd "		))	{	if (mat) mat.diffuse		= parsefloat3( line.slice(3) );		}
		else if (starts_with(line, "Ka "		))	{	if (mat) mat.ambient		= parsefloat3( line.slice(3) );		}
		else if (starts_with(line, "newmtl "	)) {
			var name = line.slice(7);
			mat = {};
			materials[ name ] = mat;
		}
	}
}

parse_lines();

return materials;
}
