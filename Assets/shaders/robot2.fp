// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 430

uniform vec4 ambientColor;
uniform int mode;
uniform float fraction;
uniform samplerCube skybox;
uniform vec3 cameraPos;
out vec4 vFragColor;

//lighting color
//vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(0.8,0.8,0.8,1);   
vec4    specularColor = vec4(1.0,1.0,1.0,1);

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec3 vPosition3;
in vec2 UV;
float Shininess = 1.0;//for material specular

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


void main(void)
{
	if(mode == 0) 
	{
		// Dot product gives us diffuse intensity
		float diff = max(0.0, dot(normalize(vVaryingNormal),
					normalize(vVaryingLightDir)));

		// Multiply intensity by diffuse color, force alpha to 1.0
		vFragColor = diff * diffuseColor * texture(texture_diffuse1, UV);

		// Add in ambient light
		vFragColor += ambientColor;


		// Specular Light
		vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
			//反射角
			
		float spec = max(0.0, dot(normalize(vVaryingNormal), vVaryingNormal));
		if(diff != 0) 
		{
			spec = pow(spec, Shininess);
			vFragColor += specularColor * texture(texture_specular1, UV) * spec;
		}
	}
	else if(mode == 1)
	{
		float intensity;
		vec4 color;
		intensity = max(0.0, dot(normalize(vVaryingLightDir), normalize(vVaryingNormal)));

		if (intensity > pow(0.95, fraction)) {
			color = vec4(vec3(1.0), 1.0);
		} else if (intensity > pow(0.5, fraction)) {
			color = vec4(vec3(0.6), 1.0);
		} else if (intensity > pow(0.25, fraction)) {
			color = vec4(vec3(0.4), 1.0);
		} else {
			color = vec4(vec3(0.2), 1.0);
		}

		vFragColor = color * texture(texture_diffuse1, UV);
	}
	else if (mode == 2)
	{
		vec3 I = normalize(vPosition3 - cameraPos);
		vec3 R = reflect(I, normalize(vVaryingNormal));
		vFragColor = vec4(texture(skybox, R).rgb, 1.0);
	}
	else if (mode == 3 || mode == 4|| mode == 5)
	{
		float ratio = 1.00 / 1.52;
		if (mode == 4) ratio = 1.00/1.00;
		else if (mode == 5) ratio = 1.00 / 2.42;
		vec3 I = normalize(vPosition3 - cameraPos);
		vec3 R = refract(I, normalize(vVaryingNormal), ratio);
		vFragColor = vec4(texture(skybox, R).rgb, 1.0);
	}
	
}
	