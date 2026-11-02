#shader vertex
#version 460 core

layout (location = 0) in vec3 pos; 

/*
* Above is an attrribute variable 
* 
* We can understand an attrribute variable to be a piece of data that is ONLY for a specific shaders!!!
* It's NOT shared acrossed other shaders!
* 
* Below is an uniform variable 
* 
* We can understand an uniform variable as a variable that provides data for multiple shaders!!!
* It can and IS used for multiple shaders!
*/

uniform mat4 model; 

void main() 
{
	gl_Position = model * vec4(0.4 * pos.x, 0.4 * pos.y, 0.4 * pos.z, 1.0); 	
};