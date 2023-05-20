var set_timeout;
;(function(){
var timeouts = []; // [future_ms, callback]
function time_now() { return new Date().getTime(); }
set_timeout = function(future_ms, callback) {
	if (isnum(future_ms) && isfun(callback)) // prevalidate to save tick burden
		timeouts.push([time_now()+future_ms, callback]);
}
var runnables = [];
hooks.set('nizaam.tick', function () {
	if (timeouts.length) {
		var now = time_now();
		timeouts = timeouts.filter(function (o) {
			if (now >= o[0]) {
				runnables.push( o[1] );
				return 0;
			}
			return 1;
		});
		// funcs are deferred in case more funcs are set while removable
		if (runnables.length) {
			runnables.forEach(function (o) {
				o();
			});
			runnables = [];
		}
	}
});
})();