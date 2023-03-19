//+hook unhook run rununtilconsumed hooks
// 04 mar 2023 made data, map, uid private; added support for +1 arguments
// 25 jul 2020 .set made name optional
// 09 aug 2020 hooks made global
// 28 sep 2020 .set hook can be an array, reg multiple hooks with the same func
// 11 jun 2018 added map-uids, events reg'd last get called last
// 10 feb 2017 created hooks
// an EventEmitter like api
// register cross module and cross app hooks
if (!hooks) {
var hooks;
;(function (){
	var getargs = function (start_at, oldargs) {
		var args = [];
		for (var i = start_at || 0; i < oldargs.length; ++i) {
			args.push( oldargs[i] );
		}
		return args;
	};
	var data = {},
		map = {}, // map names to uids
		uid = 0;
	hooks = {
		// set multiple handlers on a hook name
		// if name is a fn, then assign it to fn and then gen rand name
		set: function (hook, name, fn) {
			if (hook instanceof Array) {
				hook.forEach(function (item) {
					hooks.set(item, name, fn);
				});
				return;
			}
				
			if (typeof name === 'function')
				fn = name, name = new Date().getTime();
				
			if (typeof fn === 'function') {
				// check if there's a hook by that name
				if (data[hook] === undefined) {
					data[hook] = [];
				}
				
				++uid;
				
				// register using uid
				data[hook][uid] = fn;
				
				// map uid under name
				map[hook+'_'+name] = uid;
				return true;
			}
			return false;
		},
		// run all handlers listening on this name and pass it this extras object
		// add a try/catch clause to both run* fn's; make contigencies
		run: function (hook, extras) {
			var args = getargs(1, arguments);
			var handlers = data[hook];
			if (handlers instanceof Array) {
				for (var i in handlers) {
					if (typeof handlers[i] === 'function') {
						handlers[i].apply(handlers[i], args);
					}
				}
				return true;
			}
			return false;
		},
		// runs handlers until one of them returns anything that evals to true
		// useful for cascading events like taps
		// returns false if no handler returned true-like value
		rununtilconsumed: function (hook, extras) {
			var args = getargs(1, arguments);
			var handlers = data[hook];
			if (handlers instanceof Array) {
				for (var i in handlers) {
					if (typeof handlers[i] === 'function') {
						var returnedvalue = handlers[i].apply(handlers[i], args);
						if (returnedvalue) {
							return returnedvalue;
						}
					}
				}
			}
			return false;
		},
		pop: function (hook, name) {
			if (data[hook]) {
				
				// get its uid from map using hook+name, access using uid
				delete data[hook][ map[hook+'_'+name] ];
				return true;

			}
			return false;
		},
		// new hook, creates an empty array for now
		// TODO add options like what kinda handlers to allow/disallow
		// 		max handlers, delete if too many, tagging to group similar hooks together etc
		hook: function (hook) {
			data[hook] = [];
		},
		// remove a hook and all its listeners
		unhook: function (hook) {
			delete data[hook];
		}
	};
})();
}
