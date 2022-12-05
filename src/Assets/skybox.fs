#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform bool enableToon;

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


void main()
{    
    vec4 color = texture(skybox, TexCoords);
    if (enableToon){
        color = toonMapping(color);
    }
    FragColor = color;
}
