//clear();
require('apps/core.js');
function to_data_units(m) {
	return ( m.length * m.BYTES_PER_ELEMENT / 1000 / 1000 ) + 'MB';
}

//var thread = function () {
//	var mem = new Float32Array(1 * 1000000 / 4);
//	log('thread', (mem.length * mem.BYTES_PER_ELEMENT) / 1000 / 1000, 'MB');
////	mem = undefined;
//	return mem;
//};
//var sched = function () {
//	asio.thread(0, thread, function (d) {
//		pretty('callback', d);
//		d = undefined;
//		Duktape.compact(global);
////		sched();
////		if (d) {
////			log('main', (d.mem.length * 4) / 1000 / 1000, 'MB');
////		}
//	});
//};
////for (var i = 0; i < 8; ++i) {
//	sched();
////}

