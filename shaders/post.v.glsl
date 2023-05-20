in vec2 coord2d;
in vec2 textu2d;

out vec2 TexCoords;

void main() {
	TexCoords = textu2d;
	gl_Position = vec4(coord2d.x, coord2d.y, 0.0, 1.0); 
}  

