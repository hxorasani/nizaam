var Collision;
;(function(){
var EPSILON = 0.000001; // A small number
with_triangle = function (ray, p1, p2, p3) {
	var collision = this;
	collision.hit = false;
	var edge1 = vec3.create();
	var edge2 = vec3.create();
	var p  = vec3.create(),
		q  = vec3.create(),
		tv = vec3.create();
	var det, invDet, u, v, t; // floats

	// Find vectors for two edges sharing V1
	vec3.subtract(edge1, p2, p1);
	vec3.subtract(edge2, p3, p1);

	// Begin calculating determinant - also used to calculate u parameter
	vec3.cross(p, ray.direction, edge2);

	// If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = vec3.dot(edge1, p);

	// Avoid culling!
	if ((det > -EPSILON) && (det < EPSILON)) return collision;

	invDet = 1/det;

	// Calculate distance from V1 to ray origin
	vec3.subtract(tv, ray.position, p1);

	// Calculate u parameter and test bound
	u = vec3.dot(tv, p)*invDet;

	// The intersection lies outside the triangle
	if ((u < 0) || (u > 1)) return collision;

	// Prepare to test v parameter
	vec3.cross(q, tv, edge1);

	// Calculate V parameter and test bound
	v = vec3.dot(ray.direction, q)*invDet;

	// The intersection lies outside the triangle
	if ((v < 0) || ((u + v) > 1)) return collision;

	t = vec3.dot(edge2, q)*invDet;

	if (t > EPSILON)
	{
		// Ray hit, get hit point and normal
		collision.hit = true;
		collision.distance = t;
		vec3.cross(collision.normal, edge1, edge2);
		vec3.normalize(collision.normal, collision.normal);
		vec3.scaleAndAdd(collision.point, ray.position, ray.direction, t);
	}

	return collision;
}
with_quad = function (ray, q) { // Ray [vec3 vec3 vec3 vec3]
	/* points are expected to be in ccw winding
	 * */
	var collision = this;
	collision.hit = false;
	collision.with_triangle(ray, q[0], q[1], q[3]);
	if (!collision.hit) collision.with_triangle(ray, q[1], q[2], q[3]);
	return collision;
}
with_box = function(ray, box) {
	var collision = this;
	collision.hit = false;

	// Note: If ray.position is inside the box, the distance is negative (as if the ray was reversed)
	// Reversing ray.direction will give use the correct result.
	var insideBox  = (ray.position[0] > box.min[0]) && (ray.position[0] < box.max[0]) &&
					 (ray.position[1] > box.min[1]) && (ray.position[1] < box.max[1]) &&
					 (ray.position[2] > box.min[2]) && (ray.position[2] < box.max[2]);

	if (insideBox) vec3.negate(ray.direction, ray.direction);

	var t = new Float32Array(11);

	t[8 ] = 1/ray.direction[0];
	t[9 ] = 1/ray.direction[1];
	t[10] = 1/ray.direction[2];

	t[0] = (box.min[0] - ray.position[0])*t[8];
	t[1] = (box.max[0] - ray.position[0])*t[8];
	t[2] = (box.min[1] - ray.position[1])*t[9];
	t[3] = (box.max[1] - ray.position[1])*t[9];
	t[4] = (box.min[2] - ray.position[2])*t[10];
	t[5] = (box.max[2] - ray.position[2])*t[10];
	t[6] = Math.max(Math.max(Math.min(t[0], t[1]), Math.min(t[2], t[3])), Math.min(t[4], t[5]));
	t[7] = Math.min(Math.min(Math.max(t[0], t[1]), Math.max(t[2], t[3])), Math.max(t[4], t[5]));

	collision.hit = !((t[7] < 0) || (t[6] > t[7]));
	collision.distance = t[6];
	vec3.add(collision.point, ray.position, vec3.scale(vec3.create(), ray.direction, collision.distance));

	// Get box center point
	vec3.lerp(collision.normal, box.min, box.max, 0.5);
	// Get vector center point->hit point
	vec3.subtract(collision.normal, collision.point, collision.normal);
	// Scale vector to unit cube
	// NOTE: We use an additional .01 to fix numerical errors
	vec3.scale(collision.normal, collision.normal, 2.01);
	vec3.divide(collision.normal, collision.normal, vec3.subtract(vec3.create(), box.max, box.min));
	// The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
	// and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
	collision.normal[0] = parseint(collision.normal[0]);
	collision.normal[1] = parseint(collision.normal[1]);
	collision.normal[2] = parseint(collision.normal[2]);

	vec3.normalize(collision.normal, collision.normal);

	if (insideBox) {
		// Reset ray.direction
		vec3.negate(ray.direction, ray.direction);
		// Fix result
		collision.distance *= -1.0;
		vec3.negate(collision.normal, collision.normal);
	}

	return collision;
};

Collision = function () { // RayCollision, ray hit information
	var o = {
		hit: false,				// Did the ray hit something?
		distance: 0.0,			// Distance to the nearest hit
		point: vec3.create(),	// Point of the nearest hit
		normal: vec3.create(),	// Surface normal of hit
	};
	
	o.with_triangle	= with_triangle	;
	o.with_quad		= with_quad		;
	o.with_box		= with_box		;
	
	return o;
};

})();