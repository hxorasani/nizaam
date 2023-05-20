in vec4 f_color;
uniform int show_vertices;
uniform int picking_mode;
uniform float has_color;
uniform vec4 single_color;
in vec3 frag_pos;

uniform float farPlane;
uniform vec3 sun_pos;
uniform vec3 sun_dir;
uniform vec3 cam_pos;
uniform vec3 fogColor;

uniform vec4 vertex_outline;
uniform vec4 vertex_fill;
uniform float vertex_size;

in vec2 TexCoords;
uniform sampler2D screen_tex;

uniform int lighting;
uniform int selected;
uniform int ortho;

uniform uint object_id;

out vec4 FragColor;

//{ LIGHTING
in vec3 varyingNormalDirection;
in vec4 position;
uniform mat4 v_inv;
struct lightSource {
	vec4 position;
	vec4 diffuse;
	vec4 specular;
	float constantAttenuation, linearAttenuation, quadraticAttenuation;
	float spotCutoff, spotExponent;
	vec3 spotDirection;
};
const int numberOfLights = 1;
lightSource lights[numberOfLights];
lightSource light0 = lightSource(
	vec4(0.0,  0.0,  1.0, 0.0),
	vec4(1.0,  1.0,  1.0, 1.0),
	vec4(1.0,  1.0,  1.0, 1.0),
	0.0, 1.0, 0.0,
	180.0, 0.0,
	vec3(0.0, 0.0, 0.0)
);
struct material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
material frontMaterial = material(
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	1.0
);
vec3 calc_lighting() {
	lights[0] = light0;
	
	vec3 normalDirection = normalize(varyingNormalDirection);
	vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
	vec3 lightDirection;
	float attenuation;
	
	// initialize total lighting with ambient lighting
	vec3 totalLighting = vec3(fogColor) * vec3(frontMaterial.ambient);
  
	for (int index = 0; index < numberOfLights; index++) { // for all light sources
		vec4 light_pos = vec4(sun_pos, lights[index].position.w);
		vec3 light_dir = sun_dir;
		if (0.0 == light_pos.w) { // directional light?
			attenuation = 1.0; // no attenuation
			lightDirection = normalize(vec3(light_pos));
		} else { // point light or spotlight (or other kind of light) 
			vec3 positionToLightSource = vec3(light_pos - position);
			float distance = length(positionToLightSource);
			lightDirection = normalize(positionToLightSource);
			attenuation = 1.0 / (lights[index].constantAttenuation
						+ lights[index].linearAttenuation * distance
						+ lights[index].quadraticAttenuation * distance * distance);
			
			if (lights[index].spotCutoff <= 90.0) { // spotlight?
				float clampedCosine = max(0.0, dot(-lightDirection, normalize(light_dir)));
				if (clampedCosine < cos(radians(lights[index].spotCutoff))) { // outside of spotlight cone?
					attenuation = 0.0;
				} else {
					attenuation = attenuation * pow(clampedCosine, lights[index].spotExponent);   
				}
			}
		}
	  
		vec3 diffuseReflection = attenuation 
			* vec3(lights[index].diffuse) * vec3(frontMaterial.diffuse)
			* max(0.0, dot(normalDirection, lightDirection));
	  
		vec3 specularReflection;
		if (dot(normalDirection, lightDirection) < 0.0) { // light source on the wrong side?
			specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
		} else { // light source on the right side
			specularReflection = attenuation * vec3(lights[index].specular) * vec3(frontMaterial.specular)
				* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), frontMaterial.shininess);
		}

		totalLighting = totalLighting + diffuseReflection + specularReflection;
	}
	return totalLighting;
}
//} END LIGHTING


float getFogFactor(float d, float farPlane) {
	float FogMax = 1.0f * farPlane;
	float FogMin = 0.5f * farPlane;
	if (d>=FogMax) return 1.0f;
	if (d<=FogMin) return 0.0f;
	return 1.0f - (FogMax - d) / (FogMax - FogMin);
}
float getFogFactorAlpha(float d, float farPlane) {
	float FogMax = 1.0f * farPlane;
	float FogMin = 0.7f * farPlane;

	if (d>=FogMax) return 1.0f;
	if (d<=FogMin) return 0.0f;

	return 1.0f - (FogMax - d) / (FogMax - FogMin);
}
void main(void) {
	float d = 0, fogFactor = 0, alpha = 0;
	if (ortho != 1 && picking_mode != 1) {
		d = distance(cam_pos, frag_pos);
		fogFactor = getFogFactor(d, farPlane);
		alpha = getFogFactorAlpha(d, farPlane);
	}

	if (picking_mode == 1) {
//		FragColor = vec4(float(object_id), 0, 0, 1);
		FragColor = vec4(object_id + gl_PrimitiveID, 0, 0, 1);
	} else if (show_vertices == 1) {
		vec2 coord = gl_PointCoord - vec2(0.5); //from [0,1] to [-0.5,0.5]

		float radius = .3;
		float borderThickness = .05;
		float d = length(coord);

		float t1 = 1.0 - smoothstep(radius-borderThickness, radius, d);
		float t2 = 1.0 - smoothstep(radius, radius+borderThickness, d);

		if (t2 < .3) discard;

		vec4 col = vec4( mix(vertex_outline.rgb, vertex_fill.rgb, t1), t2 );
		FragColor = mix(col, vec4(fogColor, 1.0f), fogFactor);
	} else {
		vec4 out_color;
		if (has_color == 2) { // texture only
			vec3 col = texture(screen_tex, TexCoords).rgb;
			out_color = vec4(col, 1.0);
		} else if (has_color == 1) {
			out_color = f_color;
		} else {
			out_color = single_color;
		}

		if (selected == 1) {
			out_color = mix(out_color, vec4(1,1,1,1), 0.2f);
		}
		
		if (lighting == 1) {
			vec3 totalLighting = calc_lighting();
			out_color = vec4( mix(out_color.xyz, totalLighting, .25), 1 );
		}

		out_color = mix(out_color, vec4(fogColor, 1.0f), fogFactor);

		FragColor = out_color;
	}
}
