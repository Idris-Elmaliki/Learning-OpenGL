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
	// Here we are multiplying the 4x4 matrix we made by our previously added vec4!  

	// with glm::scale, we can scale our 4x4 matrix before passing it into the shader
	// in addition, we can just pass in our vec3, instead of calling each member indiviually!
	gl_Position = model * vec4(pos, 1.0); 	
};