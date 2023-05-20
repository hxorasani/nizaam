//+ _init _next _process run on_next on_finish uid count _didinit _didrun queuearray
//+ _on_next _on_finish finish finished
var Queue;
;(function (){
	var _queuePrototype = {
		_init: function () {
			if (!this._didinit) {
//				console.log('_init');
				this._didinit = true;
				this._didrun = false;
				
				this.queuearray = [];
				this.active = false;
				this.count = 0;
				this.uid = false;
				this._on_next = false;
				this._on_finish = false;
			}
		},
		_next: function (queue, extra) {
//			console.log('_next');
			++queue.count;
			queue._process(queue, extra);
			queue.active = false;
			return queue.count;
		},
		_process: function (queue, extra) {
//			console.log('_process');
			if (queue.finished) return;
			if (isfun(queue._on_next)) {
				if (queue.queuearray.length > 0) {
					queue.uid = queue.queuearray.length;
					queue.active = true;
					
					var worker = queue.queuearray.pop();
					var done = function (queue, extra) {
						queue._on_next(queue._next, queue, extra);
					};
					
					worker(done, queue, extra);
					
					return false;
				} else {
					queue.active = false;
					--queue.count;
					if (isfun(queue._on_finish)) {
						queue._on_finish(queue, extra);
					}
					return false;
				}
			}
		},
		finish: function (extra) {
			this.finished = 1
			if (isfun(this._on_finish)) this._on_finish(this, extra);
		},
		set: function (worker) {
//			console.log('set');
			this._init();
			
			if (isfun(worker)) {
				this.queuearray.unshift(worker);
			}
			
			return this;
		},
		run: function (_on_finish, extra) {
//			console.log('run');

			this.on_finish(_on_finish);

			var queue = this;
			queue._init();
			
			// if there's no _on_next fn set, use the built in one
			if (!isfun(queue._on_next)) {
				queue.on_next(function (done, _queue, extra) {
					done(_queue, extra);
				});
			}
			
			if (!queue._didrun && isfun(queue._on_next)) {
				queue._didrun = true;
				
				queue._process(queue, extra);
			}
		},
		on_next: function (cb) {
			if (isfun(cb)) this._on_next = cb;
		},
		on_finish: function (cb) {
			if (isfun(cb)) this._on_finish = cb;
		}
	};
	
	Queue = function (on_next, on_finish) {
		var newqueue = Object.create(_queuePrototype);
		newqueue._init();
		
		if (isfun(on_next)) newqueue._on_next = on_next;
		if (isfun(on_finish)) newqueue._on_finish = on_finish;
		
		return newqueue;
	};
})();
