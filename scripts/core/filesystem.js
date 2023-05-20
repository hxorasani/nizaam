var Filesystem;
;(function(){

	Filesystem = {
		read: function (path, callback) {
			if (isundef(callback)) {
				var fd = asio.fs_open(path, '', 0);
				if (fd) {
					var stats = asio.fs_fstat(fd);
					if (stats && stats.size) {
						var data = asio.fs_read(fd, stats.size, -1);
						asio.fs_close(fd);
						return data;
					} else {
						asio.fs_close(fd);
						return null;
					}
				} else return null;
			} else
			asio.fs_open(path, '', 0, function (fd) {
				if (fd) {
					asio.fs_fstat(fd, function (stats) {
						if (stats && stats.size) {
							asio.fs_read(fd, stats.size, -1, function (data) {
								asio.fs_close(fd);
								callback(data);
							});
						} else {
							asio.fs_close(fd);
							callback(null);
						}
					});
				} else callback(null);
			});
		},
		size: function (path, callback) {
			asio.fs_open(path, '', 0, function (fd) {
				if (fd)
				asio.fs_fstat(fd, function (stats) {
					callback(stats.size);
					
					asio.fs_close(fd);
				});
				else callback(null);
			});
		},
		dirs: function (path, callback) {
			if (isundef(callback)) {
				var userdata = asio.fs_scandir(path);
				if (path.endsWith('/')) path = path.slice(0, -1);
				var o, store = [];
				if (userdata) {
					while (o = asio.fs_scandir_next(userdata)) {
						store.push({
							name: o.name,
							type: o.type,
						});
					}
					return store;
				} else return null;
			} else
			asio.fs_scandir(path, function (userdata) {
				if (path.endsWith('/')) path = path.slice(0, -1);
				var o, store = [];
				if (userdata) {
					while (o = asio.fs_scandir_next(userdata)) {
						store.push({
							name: o.name,
							type: o.type,
						});
					}
					callback(store);
				} else callback(null);
			});
		}
	};
})();
