if (this.ease) {
ease.all = [
"linearnone",
"linearin",
"linearout",
"linearinout",
"sinein",
"sineout",
"sineinout",
"circin",
"circout",
"circinout",
"cubicin",
"cubicout",
"cubicinout",
"quadin",
"quadout",
"quadinout",
"expoin",
"expoout",
"expoinout",
"backin",
"backout",
"backinout",
"bounceout",
"bouncein",
"bounceinout",
"elasticin",
"elasticout",
"elasticinout"
];
ease.all.forEach(function (o, i, arr) {
	ease[o] = function (t, b, c, d) {
		return ease.with_name(arr.indexOf(o), t, b, c, d);
	};
});
}