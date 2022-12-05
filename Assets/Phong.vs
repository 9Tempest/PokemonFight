#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 aTexCoords;

out vec2 TexCoords;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};
uniform LightSource light;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Perspective;
uniform mat4 lightSpaceMatrix;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
uniform mat3 NormalMatrix;

out VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
	vec4 FragPosLightSpace;
	vec3 FragPos;
} vs_out;


void main() {
	vec4 pos4 = vec4(position, 1.0);
	mat4 ModelView = View * Model;
	//-- Convert position and normal to Eye-Space:
	vs_out.position_ES = (ModelView* pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);

	vs_out.light = light;

	//-- calc position for shadow
	vs_out.FragPos = vec3(Model * pos4);
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position = Perspective * ModelView * vec4(position, 1.0);
	TexCoords = vec2(aTexCoords.x, aTexCoords.y);
}
