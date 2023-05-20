out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D screen_tex;

void main() {
	vec3 col = texture(screen_tex, TexCoords).rgb;
//	float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
//	FragColor = vec4(vec3(grayscale), 1.0);
	FragColor = vec4(vec3(col), 1.0);
} 

