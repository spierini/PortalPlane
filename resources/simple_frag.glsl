#version 330 core 
in vec3 fragNor;
out vec4 color;
in vec3 diffShade;
in vec3 ambient;


void main()
{
	vec3 normal = normalize(fragNor);
	// Map normal in the range [-1, 1] to color in range [0, 1];
	vec3 Ncolor = 0.5*normal + 0.5;
    
	color = vec4(diffShade, 1.0) + vec4(ambient, 1);
}
