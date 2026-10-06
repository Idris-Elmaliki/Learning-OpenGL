#shader vertex
#version 460 core

layout (location = 0) in vec3 pos; 

/*
* This is an uniform variable 
* 
* We can imagine an uniform variable as a variable that like att 
*/
uniform float xMove; 

void main() 
{
	gl_Position = vec4(
		0.4 * pos.x + xMove, 
		0.4 * pos.y, 
		0.4 * pos.z + xMove, 
		1.0
	); 	
};