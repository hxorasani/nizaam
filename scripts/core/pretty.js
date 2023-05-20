/* TODO
 * make a func that returns an arr that 1:1 maps to given str and has colors
 * */
var colorize, pretty, beautify;
;(function(){
//	var stdout = asio.new_tty(1, false);
	// BUG this was causing clear() to not work
	var width = 80; // asio.tty_get_winsize(stdout).width;
	var theme = {
		property  : "38;5;253",
		braces    : "38;5;247",
		sep       : "38;5;240",
		undefined : "38;5;244",
		boolean   : "38;5;220", // yellow-orange
		number    : "38;5;202", // orange
		string    : "38;5;34" ,  // darker green
		quotes    : "38;5;40" ,  // green
		escape    : "38;5;46" ,  // bright green
		function  : "38;5;129", // purple
		cfunction : "38;5;161", // purple-red
		thread    : "38;5;199", // pink
		regexp    : "38;5;214", // yellow-orange
		date      : "38;5;153", // blue-purple
		null      : "38;5;27" ,  // dark blue
		object    : "38;5;27" ,  // blue
		buffer    : "38;5;39" ,  // blue2
		dbuffer   : "38;5;69" ,  // teal
		pointer   : "38;5;124", // red
		error     : "38;5;196", // bright red
		success   : "38;5;120;48;5;22",  // bright green
		failure   : "38;5;215;48;5;52",  // bright green
		highlight : "38;5;45;48;5;236",  // bright teal with grey background
	};
	function color(color_name) {
		return "\x1b[" + (color_name ? theme[color_name] : "0") + "m";
	}
	colorize = function (color_name, string, reset_name) {
		return color(color_name) + string + color(reset_name);
	}
	var quote, quote2, obracket, cbracket, obrace, cbrace, comma, colon;
	quote = colorize("quotes", '"', "string");
	quote2 = colorize("quotes", '"');
	obracket = colorize("braces", '[');
	cbracket = colorize("braces", ']');
	obrace = colorize("braces", '{');
	cbrace = colorize("braces", '}');
	comma = colorize("sep", ',');
	colon = colorize("sep", ':');
	var bufferObjectTypes = {
		Buffer: true,
		ArrayBuffer: true,  // Duktape 2.x: both plain buffer and ArrayBuffer
		Uint8Array: true,
		Uint8ClampedArray: true,
		Int8Array: true,
		Uint16Array: true,
		Int16Array: true,
		Uint32Array: true,
		Int32Array: true,
		Float32Array: true,
		Float64Array: true,
		DataView: true
	};
	var isPlainBuffer = function (v) {
		return Duktape.info(v)[0] === 7;  // Duktape 1.x and 2.x
	};
	function dumper(value, depth) {
		var seen = [];
		var i, n, t;
		var type = typeof value;
		if (type === "undefined") {
			return colorize("undefined", "undefined");
		}
		if (value === null) {
			return colorize("null", "null");
		}
		if (type === "boolean") {
			return colorize("boolean", "" + value);
		}
		if (type === "number") {
			return colorize("number", "" + value);
		}
		if (type === "string") {
			var str = JSON.stringify(value);
			return (quote + str.substring(1, str.length - 1) + quote2).
				replace(/(\\u[0-9a-f]{4}|\\["\\/bfnrt])/g, function (match) {
				return colorize("escape", match, "string");
				});
		}

		var info = Duktape.info(value);
		var fullName = Object.prototype.toString.call(value);
		var name = fullName.substring(8, fullName.length - 1);
		if (type === "function") {
			var fname = value.name || info[1];
			// Native CFunctions don't have a .prototype property.
			if (value.prototype) {
				return colorize("function", "[Function " + fname + "]");
			}
			return colorize("cfunction", "[Native " + fname + "]");
		}
		if (name === "RegExp") {
			return colorize("regexp", "[RegExp " + value + "]");
		}
		if (name === "Thread") {
			return colorize("thread", "[Thread " + info[1] + "]");
		}
		if (type === "buffer" || // Duktape 1.x plain buffer
			bufferObjectTypes[name]) {  // Duktape 2.x plain buffer, ArrayBuffer, typed array, Node.js Buffer
				var plain = value; // getPlainBuffer was resulting in err undefined not callable
				var bytes = [];
				for (i = 0, n = Math.min(value.byteLength, 10); i < n; i++) {
					t = plain[value.byteOffset + i];
					if (t !== undefined)
					bytes.push(parsefloat(t, 3));
				}
				var preview = bytes.join(" ");
				var length = colorize("undefined", '('+value.length+') ');
				if (value.byteLength > 10) { preview += "..."; }
				if (isPlainBuffer(value)) {
					if (info[4] === void 0) {
						// Fixed buffers have undefined for info[4]
						return length + colorize("buffer", "[Buffer " + preview + "]");
					} else {
						return length + colorize("dbuffer", "[Dynamic Buffer " + preview + "]");
					}
				}
				return length + colorize("buffer", "[" + name + " " + preview + "]");
		}
		if (name === "Pointer") {
			return colorize("pointer", "[Pointer " + info[1] + "]");
		}
		if (name === "Error") {
			return colorize("error", "[" + value.constructor.name + " " + value.message + "]");
		}
		if (name === "Date") {
			return colorize("date", "[Date " + value + "]");
		}
		if (name === "String") {
			return colorize("string", "[String " + JSON.stringify(value) + "]");
		}
		if (name === "Number") {
			return colorize("number", "[Number " + value + "]");
		}
		if (name !== "Object" && name !== "Array" && name !== "global") {
			return colorize("object", "[" + name + " " + info[1] + "]");
		}
	
		var index = seen.indexOf(value);
		if ((depth > 3 || index >= 0) && name !== "Array") { // dangerous recursion
			return colorize("object", "[" + name + " " + info[1] + "]");
		}
		seen.push(value);
	
		var parts, opener, closer;
		if (name === "Array") {
			opener = colorize("undefined", '('+value.length+') ') + obracket;
			closer = cbracket;
			parts = value.slice(0, 10).map(function (item) {
				return dumper(item, depth); // was depth + 1, caused undef BUG
			});
			if (value.length > 10) parts.push( '...' );
		}
		else {
			opener = obrace;
			closer = cbrace;
			parts = Object.keys(value).map(function (key) {
				return colorize("property", key) + colon + " " + dumper(value[key], depth + 1);
			});
		}
	
		var line = opener + " " + parts.join(comma + " ") + " " + closer;
		var max = width - depth * 2;
		if (strip(line).length > max) {
			var lines = [];
			line = [];
			max -= 2;
			var left = max;
			parts.forEach(function (part) {
				var len = strip(part).length + 2;
				if (left < len) {
					if (line.length) {
						lines.push(line);
					}
					left = max;
					line = [];
				}
				line.push(part);
				left -= len;
			});
			if (line.length) {
				lines.push(line);
			}
			lines = lines.map(function (line) {
				return line.join(comma + " ");
			});
		
			line = opener + "\n  " + lines.join(comma + "\n").split("\n").join("\n  ") + "\n" + closer;
		}
	
		return line;
	}
	function dump(value) {
		return dumper(value, 0);
	}
	
	function strip(string) {
		return string.replace(/\x1b\[[^m]*m/g, '');
	}
	
	beautify = function () {
		return Array.prototype.map.call(arguments, dump).join(" ");
	};
	pretty = function () {
		log( beautify.apply(null, arguments) );
	};
	pretty.theme = theme;
})();
