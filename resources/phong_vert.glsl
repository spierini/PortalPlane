#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 V;

out vec3 fragNor;//'N'
out vec3 worldPos;

uniform float lightPos;//shift light in x direction

uniform vec3 MatDif;//Kd
uniform vec3 MatAmb;//Ka
uniform vec3 MatSpec;//Ks
uniform float shine;//n

void main()
{
    // normalized vector 'N'
    fragNor = (V * MV * vec4(vertNor, 0.0)).xyz;
    worldPos = vec3(V * MV * vertPos);
    
    gl_Position = P * V * MV * vertPos;
        
}
