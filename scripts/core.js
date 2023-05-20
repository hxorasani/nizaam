var core_loaded = core_loaded || 0;
if (!core_loaded) {
	var p = Nizaam.scripts_path;
	require(p+'scripts/gl-matrix.min.js');
	require(p+'scripts/core/reset.js'); // TODO require should check for dup
	require(p+'scripts/core/hooks.js');
	require(p+'scripts/core/queue.js');
	require(p+'scripts/core/v3.js'); // vector math, add neg copy ...
	require(p+'scripts/core/colors.js'); // rgb array colors like c_black c_white ...
	require(p+'scripts/core/filesystem.js');
	require(p+'scripts/core/timers.js');
	require(p+'scripts/core/pretty.js');
	require(p+'scripts/core/text.js');
	require(p+'scripts/eventcodes.js');
	require(p+'scripts/core/show_keys.js');
//	require(p+'scripts/core/show_input.js');
	core_loaded = 1;
}