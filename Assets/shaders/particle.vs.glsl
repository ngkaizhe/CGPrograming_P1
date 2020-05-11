#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 objPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	// write my own translation model matrix
    mat4 model =  mat4(1, 0.0, 0.0, 0.0, 
					  0.0, 1, 0.0, 0.0, 
					  0.0, 0.0, 1, 0.0,  
					  objPos[0], objPos[1], objPos[2], 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}