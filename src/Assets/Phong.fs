#version 330 core

uniform bool enabletexture;
uniform bool enableToon;
uniform bool enableShadow;
uniform bool is2d;
uniform sampler2D texture1;
uniform sampler2D shadow;

in vec2 TexCoords;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
    vec4 FragPosLightSpace;
	vec3 FragPos;
} fs_in;


out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow_factor =0.0;

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    vec2 texelSize = 1.0 / textureSize(shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow_factor += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow_factor /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow_factor = 0.0;

    return shadow_factor;
}

vec4 toonMapping(vec4 color) {
    vec4 res = color;
    // red part
    if (color.r > 0.8)
		res.r = 1.0;
	else if (color.r  > 0.6)
		res.r = 0.6;
	else if (color.r  > 0.3)
		res.r = 0.3;
    else
		res.r = 0.1;
    
    // green part
    if (color.g > 0.8)
		res.g = 1.0;
	else if (color.g  > 0.6)
		res.g = 0.6;
	else if (color.g  > 0.3)
		res.g = 0.3;
    else
		res.g = 0.1;

    // blue part
    if (color.b > 0.8)
		res.b = 1.0;
	else if (color.b  > 0.6)
		res.b = 0.6;
	else if (color.b  > 0.3)
		res.b = 0.3;
    else
		res.b = 0.1;
    return res;
}

vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
    vec3 color;
    if (enabletexture){
        color = vec3(texture(texture1, TexCoords));
    }   else {
        color = material.kd;
    }
    
	diffuse = color * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    // bias
    float bias = max(0.05 * (1.0 -n_dot_l), 0.05);  

    // calculate shadow
    float shadow_factor = 0.0f;
    if (enableShadow){
        shadow_factor = ShadowCalculation(fs_in.FragPosLightSpace, bias);
    } 

    return ambientIntensity + ((1.0-shadow_factor) * light.rgbIntensity) * (diffuse+specular);
}

void main() {
    vec4 color;
    if (is2d){
        if (enabletexture){
            color = texture(texture1, TexCoords);
            if (color.a < 0.25 ) discard;
        }   else {
             color = vec4(material.kd, 1.0);
        }   // if
    }   else {
        color = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    }

    // if enable toon shading
    if (enableToon){
        color = toonMapping(color);
    }
    
    fragColour = color;
}
