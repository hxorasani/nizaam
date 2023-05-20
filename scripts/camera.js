function backtrace() { // TODO move to som better place
    var i, t, o = '';
    for (i = -3; ; i--) {
        t = Duktape.act(i);
        if (!t) { break; }
        o += '\n  '+(t.lineNumber+' \t '+t.function.name);
    }
	return o;
}
/* TODO
 * predict yaw & pitch based on pointer movement, lock them in per hold
 * */
var Cameras = {}, Ray, Vector3Unproject, quat_transformMat4,
//{ Camera system modes
DEG2RAD  = Math.PI / 180,
RAD2DEG  = 180 / Math.PI,
CAMERA_CULL_DISTANCE_NEAR   =   0.05,
CAMERA_CULL_DISTANCE_FAR    = 2500.0,

CAMERA_CUSTOM = 0,
CAMERA_FREE = 1,
CAMERA_FIRST_PERSON = 2,
CAMERA_THIRD_PERSON = 3,
// Camera projection modes
CAMERA_PERSPECTIVE = 0,
CAMERA_ORTHOGRAPHIC = 1,

CAMERA_ZOOM_MIN = 150,
CAMERA_ZOOM_MAX = 1500,

CAMERA_MOVE_SPEED			=0.4  ,
CAMERA_ROTATION_SPEED		=0.005  ,

// Camera mouse movement sensitivity
CAMERA_MOUSE_MOVE_SENSITIVITY		=0.16  ,
CAMERA_MOUSE_ROTATE_SENSITIVITY		=0.003 ,
CAMERA_MOUSE_SCROLL_SENSITIVITY		=1     ;
//}
var Camera, CAMERA = scene.type('camera'), get_active_camera, default_cam,
	is_camera_action_active;
;(function(){
var current, guid = 1; // active camera
var uni = {};
hooks.set('glsl.ready', function () {
	uni = {
		farPlane	: sgl.get_uniform("farPlane", glsl.pre),
		v_inv		: sgl.get_uniform("v_inv", glsl.pre),
		view		: sgl.get_uniform("view", glsl.pre),
		cam_pos		: sgl.get_uniform("cam_pos", glsl.pre),
		projection	: sgl.get_uniform("projection", glsl.pre),
	};
});
get_active_camera = function () { return current; }
quat_transformMat4 = function (out, q, m) { // quat, quat, mat4
	out[0] = m[0]*q[0] + m[4]*q[1] + m[ 8]*q[2] + m[12]*q[3];
	out[1] = m[1]*q[0] + m[5]*q[1] + m[ 9]*q[2] + m[13]*q[3];
	out[2] = m[2]*q[0] + m[6]*q[1] + m[10]*q[2] + m[14]*q[3];
	out[3] = m[3]*q[0] + m[7]*q[1] + m[11]*q[2] + m[15]*q[3];
};
Vector3Unproject = function(x, y, z, projection, view) { // vec3 []
	// Calculate unprojected matrix (multiply view matrix by projection matrix) and invert it
	var matViewProj = mat4.create();
	mat4.mul(matViewProj, view, projection); // MatrixMultiply(view, projection);

	// Calculate inverted matrix -> MatrixInvert(matViewProj);
	var matViewProjInv = mat4.create();
	mat4.invert(matViewProjInv, matViewProj);

	// Create quaternion from source point
	var q = quat.fromValues( x, y, z, 1 );

	// Multiply quat point by unprojecte matrix
	var qtransformed = quat.create(); // QuaternionTransform(quat, matViewProjInv)
	quat_transformMat4(qtransformed, q, matViewProjInv);

	// Normalized world points in vectors
	var result = vec3.fromValues(
		qtransformed[0]/qtransformed[3],
		qtransformed[1]/qtransformed[3],
		qtransformed[2]/qtransformed[3]
	);

	return result;
};
// Returns the cameras forward vec3 (normalized)
function GetCameraForward() {
	var camera = this;
	var forward = vec3.create();
	vec3.subtract(forward, camera.target, camera.location);
	vec3.normalize(forward, forward);
	return forward;
}
// Returns the cameras up vector (normalized)
// Note: The up vector might not be perpendicular to the forward vector
function GetCameraUp() {
	var camera = this;
	var up = vec3.create();
	vec3.normalize(up, camera.up);
	return up;
}
// Returns the cameras right vector (normalized)
function GetCameraRight() {
	var camera = this;
	var forward = camera.GetCameraForward();
	var up = camera.GetCameraUp();
	var right = vec3.create();
	vec3.cross(right, forward, up);
	return right;
}
// Moves the camera in its forward direction
function CameraMoveForward(distance, moveInWorldPlane, move_target) { // float, bool, bool
	var camera = this; camera.dirty = 1;
	var forward = camera.GetCameraForward();

	if (moveInWorldPlane) {
		// Project vector onto world plane
		forward[1] = 0;
		vec3.normalize(forward, forward);
	}

	// Scale by distance
	vec3.scale(forward, forward, distance);

	// BUG why does forward[2] eventually become zero?
//	pretty(forward[0], forward[1], forward[2], distance);

	// Move location and target
	vec3.add(camera.location, camera.location, forward);
	if (move_target)
	vec3.add(camera.target  , camera.target  , forward);

	if (isfun(camera.on_move)) camera.on_move(camera.location);
}
// Moves the camera in its up direction
function CameraMoveUp(distance) { // float
	var camera = this; camera.dirty = 1;
	var up = camera.GetCameraUp();

	// Scale by distance
	vec3.scale(up, up, distance);

//	pretty(up[0], up[1], up[2], distance);

	// Move location and target
	vec3.add(camera.location, camera.location, up);
	vec3.add(camera.target  , camera.target  , up);
}
// Moves the camera target in its current right direction
function CameraMoveRight(distance, moveInWorldPlane) {
	var camera = this; camera.dirty = 1;
	var right = camera.GetCameraRight();

	if (moveInWorldPlane) {
		// Project vector onto world plane
		right[1] = 0;
	}
	vec3.normalize(right, right);

//	pretty(right[0], right[1], right[2], distance);

	// Scale by distance
	vec3.scale(right, right, distance);

	// Move location and target
	vec3.add(camera.location, camera.location, right);
	vec3.add(camera.target  , camera.target  , right);
}
// Moves the camera location closer/farther to/from the camera target
function CameraMoveToTarget(delta, min, max) { // float
	var camera = this; camera.dirty = 1;
	var distance = vec3.distance(camera.location, camera.target);

	// Apply delta
	distance += delta;
	
	min = min || camera.min_zoom;
    max = max || camera.max_zoom;

	// Distance must be greater than 0
	if (distance < min) distance = min;
	if (distance > max) distance = max;

	// Set new distance by moving the location along the forward vector
	var forward = camera.GetCameraForward();
	vec3.scale(forward, forward, -distance);
	vec3.add(camera.location, camera.target, forward);
}
// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its location
// Note: angle must be provided in radians
function CameraYaw(angle, rotateAroundTarget) { // float bool
	var camera = this; camera.dirty = 1;
	// Rotation axis
	var up = camera.GetCameraUp();

	// View vector
	var target_location = vec3.create();
	vec3.subtract(target_location, camera.target, camera.location);

	// Rotate view vector around up axis
	Vector3RotateByAxisAngle(target_location, up, angle);

	if (rotateAroundTarget) {
		// Move location relative to target
		vec3.subtract(camera.location, camera.target, target_location);
	} else { // rotate around camera.location
		// Move target relative to location
		vec3.add(camera.target, camera.location, target_location);
	}
}
// Rotates the camera around its right vector
// Pitch is "looking up and down"
// lockView prevents camera overrotation (aka "somersaults")
// If rotateAroundTarget is false, the camera rotates around its location
// rotateUp rotates the up direction as well (typically only usefull in CAMERA_FREE)
// Note: angle must be provided in radians
function CameraPitch(angle, lockView, rotateAroundTarget, rotateUp) { // float bool bool bool
	var camera = this; camera.dirty = 1;
	// Up direction
	var up = camera.GetCameraUp();

	// View vector
	var target_location = vec3.create();
	vec3.subtract(target_location, camera.target, camera.location);

	if (lockView) {
		// In these camera modes we clamp the Pitch angle
		// to allow only viewing straight up or down.

		// Clamp view up
		var max_angle_up = vec3.angle(up, target_location);
		max_angle_up -= 0.001; // avoid numerical errors
		if (angle > max_angle_up) angle = max_angle_up;

		// Clamp view down
		var max_angle_down = vec3.angle(vec3.negate(up, up), target_location);
		max_angle_down *= -1; // downwards angle is negative
		max_angle_down += 0.001; // avoid numerical errors
		if (angle < max_angle_down) angle = max_angle_down;

			 if (angle < 0 && max_angle_down > camera.min_pitch) angle = 0;
		else if (angle > 0 && max_angle_up   < camera.max_pitch) angle = 0;

	}

	// Rotation axis
	var right = camera.GetCameraRight();

	// Rotate view vector around right axis
	Vector3RotateByAxisAngle(target_location, right, angle);

	if (rotateAroundTarget) {
		// Move location relative to target
		vec3.subtract(camera.location, camera.target, target_location);
	} else { // rotate around camera.location
		// Move target relative to location
		vec3.add(camera.target, camera.location, target_location);
	}

	if (rotateUp) {
		// Rotate up direction around right axis
		Vector3RotateByAxisAngle(camera.up, right, angle);
	}
}
// Rotates the camera around its forward vector
// Roll is "turning your head sideways to the left or right"
// Note: angle must be provided in radians
function CameraRoll(angle) { // float
	var camera = this; camera.dirty = 1;
	// Rotation axis
	var forward = camera.GetCameraForward();

	// Rotate up direction around forward axis
	Vector3RotateByAxisAngle(camera.up, forward, angle);
}
// Returns the camera view matrix
function GetCameraViewMatrix() {
	var camera = this;
	mat4.identity(camera.view_matrix);
	mat4.lookAt(
		camera.view_matrix,
		camera.location,
		camera.target,
		camera.up);
	return camera.view_matrix;
}
// Returns the camera projection matrix
var center_of_world = vec3.clone([0,0,0]);
function GetCameraProjectionMatrix() {
	var camera = this;
	mat4.identity(camera.projection_matrix);
	if (camera.projection == CAMERA_PERSPECTIVE) {
		mat4.perspective(camera.projection_matrix, camera.fovy*DEG2RAD, camera_aspect, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
	} else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
		var top = vec3.distance(center_of_world, camera.location) / 2.5;
		var right = top*camera_aspect;
		mat4.ortho(camera.projection_matrix, -right, right, -top, top, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
	}
	return camera.projection_matrix;
}
function get_projection_matrix_nosave(camera, near, far) {
	near = near || CAMERA_CULL_DISTANCE_NEAR;
	far = far || CAMERA_CULL_DISTANCE_FAR;
	projection_matrix = mat4.create();
	if (camera.projection == CAMERA_PERSPECTIVE) {
		mat4.perspective(projection_matrix, camera.fovy*DEG2RAD, camera_aspect, near, far);
	} else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
		var top = vec3.distance(center_of_world, camera.location) / 2.5;
		var right = top*camera_aspect;
		mat4.ortho(projection_matrix, -right, right, -top, top, near, far);
	}
	return projection_matrix;
}
Ray = function (p, d) {
	return {
		position: p || vec3.create(),	// Ray position (origin)
		direction: d || vec3.create(),	// Ray direction
	};
};
function camera_apply() {
	var camera = this;
	if (camera.dirty) {
		camera.dirty = 0;
		camera.GetCameraViewMatrix();
		camera.GetCameraProjectionMatrix();
		mat4.invert(camera.v_inv_matrix, camera.get_view());
		hooks.run('camera.transform', camera);
	}
	sgl.uniform_1f(uni.farPlane, CAMERA_CULL_DISTANCE_FAR);
	sgl.uniform_1i(uniforms.ortho, camera.projection == CAMERA_ORTHOGRAPHIC ? 1 : 0);
	sgl.uniform_3fv(uni.cam_pos , camera.location);
	sgl.uniform_matrix_4fv(uni.v_inv, camera.v_inv_matrix);
	sgl.uniform_matrix_4fv(uni.view, camera.view_matrix);
	sgl.uniform_matrix_4fv(uni.projection, camera.projection_matrix);
}
function camera_update() {
	var camera = this;
	camera.up	 = vec3.fromValues(0, 1, 0); // Reset roll
	camera.dirty = 1;
	camera.GetCameraViewMatrix();
	camera.GetCameraProjectionMatrix();
	mat4.invert(camera.v_inv_matrix, camera.get_view());
}
function camera_resize() {
	var camera = this;
	camera.bbox = BoundingBox(
		camera.project(0, 0),
		camera.project(Window.w, Window.h)
	);
}

var direction_names = {
	'3': 'right', '-3': 'left', '1': 'front', '-1': 'back'
};
// TODO mark dirty on update, update on actual render, mark clean
var view = mat4.create(), projection = mat4.create();
function project(x, y, mat, far) {
	var current = this;
	x = (2 * x) / Window.w - 1;
	y = 1 - (2 * y) / Window.h;
	var ray = vec3.fromValues(x, y, 1);
	var distance = far || vec3.distance(current.location, current.target);
	
	var proj = get_projection_matrix_nosave(current, .1, distance);
	mat4.invert(proj, proj);
	vec3.transformMat4(ray, ray, proj);

	var mod = current.get_view();
	mat4.invert(mod, mod);
	vec3.transformMat4(ray, ray, mod);

	if (mat) {
		mat4.invert(mat, mat);
		vec3.transformMat4(ray, ray, mat);
	}

	return ray;
}
function unproject(x, y, z, mat) {
	var current = this;
	var ray = vec3.fromValues(x, y, z);
	
	if (mat) vec3.transformMat4(ray, ray, mat);

	var mod = current.get_view();
	vec3.transformMat4(ray, ray, mod);

	var proj = current.get_projection();
	vec3.transformMat4(ray, ray, proj);

	var px = {};
	px.x = (ray[0] * .5 + .5) * Window.w;
	px.y = (.5 - ray[1] * .5) * Window.h;
	px.z = z;
	return px;
}
function raycast(x, y) {
	var camera = this;
	/* ALGORITHM
	 * viewport 					1920 x 1080
	 * normalized device			-1 : 1
	 * homogeneous clip				xyz * frustum_shape; w = -z
	 * eye space					camera is the origin
	 * world space					world origin, meshes placed in world
	 * local space					local origin
	 * */
	var ray = Ray();

	x = (2*x) / Window.w - 1;
	y = 1 - (2*y) / Window.h;
	var z = 1;

	var view = camera.get_view(),
		projection = camera.get_projection(),
		direction = ray.direction,
		position = ray.position;

	// Unproject far/near points
	var nearPoint = raymath_unproject( [x, y, 0], projection, view );
	var farPoint  = raymath_unproject( [x, y, 1], projection, view );

	// Calculate normalized direction vector
	vec3.subtract(direction, nearPoint, farPoint);

	if (camera.projection == CAMERA_PERSPECTIVE) { // perspective
		ray.position = vec3.clone(camera.location);
		vec3.negate(direction, direction);
	} else {
		// Unproject the mouse cursor in the near plane.
		// We need this as the source position because orthographic projects, compared to perspective doesn't have a
		// convergence point, meaning that the "eye" of the camera is more like a plane than a point.
		var cameraPlanePointerPos = raymath_unproject( [x, y, 1], projection, view );
		ray.position = cameraPlanePointerPos; // ortho
	}

	vec3.normalize(direction, direction);

	return ray;
}
function update_dir() {
	var camera = this;
	if (!camera.ready) return;
	var fx = camera.GetCameraForward()[0];
	var rx = camera.GetCameraRight  ()[0];

	camera.direction = 0;
	if (rx == 0) {
			 if (fx == -1) camera.direction =  3; // right
		else if (fx ==  1) camera.direction = -3; // left
	} else if (fx == 0) {
			 if (rx ==  1) camera.direction =  1; // front
		else if (rx == -1) camera.direction = -1; // back
	}
}
function activate() {
	if (current) current.active = 0;
	current = this;
	current.active = 1;
	sgl.uniform_1i(uniforms.ortho, current.projection == CAMERA_ORTHOGRAPHIC ? 1 : 0);
}
function get_projection() { return mat4.clone(this.projection_matrix); }
function get_view() { return mat4.clone(this.view_matrix); }

Camera = function (name, loc, rot, sca) {
	var _ = {
		name	: name || ++guid,

		ready : 0,
		dirty		: 1				,
		apply		: camera_apply	,
		resize		: camera_resize	,
		update		: camera_update	,
		activate	: activate		,
		project		: project		,
		unproject	: unproject		,
		raycast		: raycast		,
		update_dir	: update_dir	,
		
		disabled	: 0,
		
		up			:	vec3.fromValues(0, 1, 0),	// up vector (rotation over its axis)
		fovy		:	45,							// field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
		projection	:	CAMERA_PERSPECTIVE	,
		mode		:	CAMERA_FREE			,
		
		view_matrix			: mat4.create()		,
		v_inv_matrix		: mat4.create()		,
		projection_matrix	: mat4.create()		,
		
		get_view			: get_view			,
		get_projection		: get_projection	,
		
		min_zoom : CAMERA_ZOOM_MIN,
		max_zoom : CAMERA_ZOOM_MAX,
		
		near: CAMERA_CULL_DISTANCE_NEAR,
		far : CAMERA_CULL_DISTANCE_FAR ,
		
		GetCameraForward	: GetCameraForward	,
		GetCameraUp			: GetCameraUp		,
		GetCameraRight		: GetCameraRight	,
		
		CameraMoveForward	: CameraMoveForward	,
		CameraMoveUp		: CameraMoveUp		,
		CameraMoveRight		: CameraMoveRight	,
		CameraMoveToTarget	: CameraMoveToTarget,
		
		CameraYaw			: CameraYaw			,
		CameraPitch			: CameraPitch		,
		CameraRoll			: CameraRoll		,
		
		GetCameraViewMatrix			: GetCameraViewMatrix		,
		GetCameraProjectionMatrix	: GetCameraProjectionMatrix	,
		
		rotation : v3(0)					, // TODO this should rotate the target around location
	};
	if (loc) _.location = vec3.clone(loc);
	if (rot) _.rotation = vec3.clone(rot);
	if (sca) _.scale	= vec3.clone(sca);
	_ = scene.add('camera.'+_.name, CAMERA, _);
	const handler = {
		set: function (o, p, v) {
			o[p] = v;
			_.update_dir();
//			_.update();
			return 1;
		},
	};
	Cameras[_.guid] = _;
	var proxy = new Proxy(_, {
		set: function (o, p, v) {
			if (['location', 'target'].indexOf(p) > -1) {
				o[p] = new Proxy(v, handler);
			} else {
				if (p == 'min_pitch') v =     -v    * DEG2RAD;
				if (p == 'max_pitch') v = (180-v)   * DEG2RAD;
				o[p] = v;
			}
			return 1;
		}
	});
	// degs (auto-conv'd to rads) TODO write getter
	proxy.min_pitch = 20;
	proxy.max_pitch = 80;
	// to allow proxy to take over
	proxy.location	=	vec3.fromValues(0, 4,30);	// location
	proxy.target	=	vec3.fromValues(0, 4, 0);	// target it looks-at
	_.ready = 1;
	return proxy;
};

default_cam = Camera( 'default' );
//default_cam.location = vec3.clone( [-19,8,34] );
default_cam.location[1] = CAMERA_ZOOM_MIN*3;
default_cam.location[2] = CAMERA_ZOOM_MIN*3;
default_cam.target[1] = 0;
//default_cam.projection = CAMERA_ORTHOGRAPHIC;
default_cam.activate();
default_cam.update_dir();

var cameraMode = CAMERA_FIRST_PERSON, action, camera_aspect, dup_target;
is_camera_action_active = function () {
	return action;
};
hooks.set('nizaam.pointer', function (m) { if (Pointer.last && current && !current.disabled) {
	m = shallowcopy(m);
	var dx = Pointer.last.x-m.x;
	var dy = Pointer.last.y-m.y;

	var mode = current.mode;
	if (m.alt &&  m.shift && m.state == -1) action = MOVE, yes = 1;
	if (m.alt && !m.shift && m.state == -1) action = ROTATE, yes = 1;

	// TODO release panning in away that allows transforming while panning
	if (((!m.alt && !m.shift) || m.state == 1) && action) {
		action = 0; Pointer.last = 0;
	}

	var moveInWorldPlane = 1;
	var rotateAroundTarget = 1;
	var lockView = 1;
	var rotateUp = 0;
	var direction_y = Math.round( current.GetCameraForward()[1] ); // -1top 0rest
	// -1r 1l 0fb

	if (action == MOVE) {
		var distance = vec3.distance(current.location, current.target) / 100;
		dx *= distance;
		dy *= distance;

		if (current.projection == CAMERA_ORTHOGRAPHIC && direction_y == 0) {
			if (dy < 0) current.CameraMoveUp( CAMERA_MOUSE_MOVE_SENSITIVITY*-dy );
			if (dy > 0) current.CameraMoveUp(-CAMERA_MOUSE_MOVE_SENSITIVITY* dy );
		} else {
			if (dy < 0) current.CameraMoveForward( CAMERA_MOUSE_MOVE_SENSITIVITY*-dy, moveInWorldPlane, 1);
			if (dy > 0) current.CameraMoveForward(-CAMERA_MOUSE_MOVE_SENSITIVITY* dy, moveInWorldPlane, 1);
		}

		if (dx < 0) current.CameraMoveRight  (-CAMERA_MOUSE_MOVE_SENSITIVITY*-dx, moveInWorldPlane);
		if (dx > 0) current.CameraMoveRight  ( CAMERA_MOUSE_MOVE_SENSITIVITY* dx, moveInWorldPlane);
	}
	if (action == ROTATE) {
		if (dy > 0) current.CameraPitch( CAMERA_MOUSE_ROTATE_SENSITIVITY* dy, lockView, rotateAroundTarget, rotateUp);
		if (dy < 0) current.CameraPitch(-CAMERA_MOUSE_ROTATE_SENSITIVITY*-dy, lockView, rotateAroundTarget, rotateUp);
		if (dx < 0) current.CameraYaw  ( CAMERA_MOUSE_ROTATE_SENSITIVITY* dx, rotateAroundTarget);
		if (dx > 0) current.CameraYaw  (-CAMERA_MOUSE_ROTATE_SENSITIVITY*-dx, rotateAroundTarget);
	}
	if (mode == CAMERA_FREE && m.w) {
		current.CameraMoveToTarget(CAMERA_MOUSE_SCROLL_SENSITIVITY * m.w * 50);
	}
} if (action) return 1; });
hooks.set('scene.remove', function (o) { if (o.type == CAMERA) {
	delete Cameras[o.guid];
} });
hooks.set('scene.info', function (o) { if (o.type == CAMERA) {
	var str = '';
	var l = v3.precision_float_array( o.location );
	var r = v3.precision_float_array( o.target   );
	str += 'loc x'+l[0]+' y'+l[1]+' z'+l[2]
		+'; tgt x'+r[0]+' y'+r[1]+' z'+r[2]
		+'; fov '+o.fovy
		+(o.projection == CAMERA_ORTHOGRAPHIC ? '\northo' : '');
	return str;
} });
hooks.set('scene.pressed', function (o, k, m, held) { if (o.type == CAMERA && m.state && current && !current.disabled) {
	var yes, speed = m.shift ? 8 : m.alt ? 1 : 4;
	var mode = current.mode;
	var moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
	var lockView = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));

	if (k == '1') current.mode = CAMERA_FREE, yes = 1;
	if (k == '2') current.mode = CAMERA_FIRST_PERSON, yes = 1;
	if (k == '3') current.mode = CAMERA_THIRD_PERSON, yes = 1;
	if (scene.selected.length) {
		var obj = scene.get(scene.selected[0]);
		if (obj.type != CAMERA) {
			var tar = obj.get('location');
			current.target = vec3.fromValues(tar.x, tar.y, tar.z);
		}
	}
	// Camera movement
//	if (k == 'up'  ) current.CameraMoveUp(  CAMERA_MOVE_SPEED );
//	if (k == 'down') current.CameraMoveUp( -CAMERA_MOVE_SPEED );
	if (!m.ctrl) {
		if (held['q'	]) current.CameraRoll	( -CAMERA_ROTATION_SPEED	);
		if (held['e'	]) current.CameraRoll	( CAMERA_ROTATION_SPEED		);
	}
	if (yes) current.update();
} });
hooks.set('nizaam.keyboard', function (m, k, held) { if (m.state && current && !current.disabled) {
	if (scene.console_mode) return;
	var yes, speed = m.shift ? 8 : m.alt ? .5 : 4;
	if (k == 'grave') {
		if ( scene.includes(current.guid) ) {
			scene.deselect( current.guid );
			scene.edit_mode = 0;
		} else {
			scene.escape();
			scene.select( current.guid );
			scene.edit_mode = 1;
		}
	}
	var mode = current.mode;
	var lockView = 1;
	if (m.ctrl) {
		// Camera rotation
		if (k == 'pageup'  ) current.CameraMoveUp(  CAMERA_MOVE_SPEED * speed );
		if (k == 'pagedown') current.CameraMoveUp( -CAMERA_MOVE_SPEED * speed );
		if (held['down'	]) current.CameraPitch	(  CAMERA_ROTATION_SPEED * speed, lockView, 1, 0 );
		if (held['up'	]) current.CameraPitch	( -CAMERA_ROTATION_SPEED * speed, lockView, 1, 0 );
		if (held['right']) current.CameraYaw	(  CAMERA_ROTATION_SPEED * speed, 1	);
		if (held['left'	]) current.CameraYaw	( -CAMERA_ROTATION_SPEED * speed, 1	);
	} else {
		var direction_y = Math.round( current.GetCameraForward()[1] ); // -1top 0rest

		var distance = vec3.distance(current.location, current.target) / 200;
		speed *= distance;

		if (current.projection == CAMERA_ORTHOGRAPHIC && direction_y == 0) {
			if (held['up']) current.CameraMoveUp  (  CAMERA_MOVE_SPEED * speed, 1, 1 );
			if (held['down']) current.CameraMoveUp( -CAMERA_MOVE_SPEED * speed, 1, 1 );
		} else {
			if (held['up']) current.CameraMoveForward  (  CAMERA_MOVE_SPEED * speed, 1, 1 );
			if (held['down']) current.CameraMoveForward( -CAMERA_MOVE_SPEED * speed, 1, 1 );
		}
		if (held['right']) current.CameraMoveRight (  CAMERA_MOVE_SPEED * speed, 1, 1 );
		if (held['left']) current.CameraMoveRight  ( -CAMERA_MOVE_SPEED * speed, 1, 1 );
	}

	if (k == 'backspace') current.target = vec3.fromValues(0, 0, 0), current.fovy = 45, yes = 1;
	// TODO provide a func to override defaults per camera aka pose
	if (k == '0') current.location = vec3.fromValues(0, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MIN), yes = 1;
	// TODO allow gliding across terrain
	if (held['equal']) current.CameraMoveToTarget( -CAMERA_MOUSE_SCROLL_SENSITIVITY * speed );
	if (held['minus']) current.CameraMoveToTarget(  CAMERA_MOUSE_SCROLL_SENSITIVITY * speed );
	if (k == 'kpdot') {
		if (scene.selected.length) {
			var obj = scene.get(scene.selected[0]);
			if (obj.type != CAMERA) {
				var tar = obj.get('location');
				current.target = vec3.fromValues(tar.x, tar.y, tar.z);
			}
		}
		yes = 1;
	};
	if (k == 'kp5') current.projection = current.projection == CAMERA_PERSPECTIVE ? CAMERA_ORTHOGRAPHIC : CAMERA_PERSPECTIVE, yes = 1;
	if (k == 'kp1') { // front
		var distance = vec3.distance(current.location, current.target);
		current.location = vec3.clone([0, 0, m.ctrl ? -distance : distance]);
		current.target = vec3.clone([0, 0, 0]);
		current.projection = CAMERA_ORTHOGRAPHIC;
		yes = 1;
	}
	if (k == 'kp3') { // right
		var distance = vec3.distance(current.location, current.target);
		current.location = vec3.clone([m.ctrl ? -distance : distance, 0, 0]);
		current.target = vec3.clone([0, 0, 0]);
		current.projection = CAMERA_ORTHOGRAPHIC;
		yes = 1;
	}
	if (k == 'kp7') { // top
		var distance = vec3.distance(current.location, current.target);
		current.location = vec3.clone([0, m.ctrl ? -distance : distance, 0.001]);
		current.target = vec3.clone([0, 0, 0]);
		current.projection = CAMERA_ORTHOGRAPHIC;
		yes = 1;
	}
	if (yes) current.update();
} });
hooks.set('nizaam.resize', function (w, h) {
	sgl.viewport(0, 0, w, h);
	camera_aspect = 1 * w/h;
	for (var i in Cameras) {
		Cameras[i].resize();
		Cameras[i].update();
	}
});
hooks.set('nizaam.rotate', function (m) { if (current && !current.disabled) { // TODO temp locks
	var distance = vec3.distance(current.location, current.target) / 100;
	var deg = matrix.to_degrees(m.w);
	deg *= distance * -1;

	if (deg > 0) current.CameraYaw	( -CAMERA_ROTATION_SPEED * deg, 1 );
	if (deg < 0) current.CameraYaw	( -CAMERA_ROTATION_SPEED * deg, 1 );
} });
hooks.set('nizaam.pinch', function (m) { if (current && !current.disabled) {
	var distance = vec3.distance(current.location, current.target) / 100;
	m.w *= distance * -1000;

	if (m.state > 0) current.CameraMoveToTarget( CAMERA_MOUSE_SCROLL_SENSITIVITY * m.w);
	if (m.state < 0) current.CameraMoveToTarget( CAMERA_MOUSE_SCROLL_SENSITIVITY * m.w);
} });
hooks.run('camera.ready');
})();

