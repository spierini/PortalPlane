#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 V;

out vec3 fragNor;

out vec3 diffShade;
out vec3 ambient;
uniform vec3 Kd;
uniform int followRot;

void main()
{
    vec3 wPos = vec3(V*MV*vertPos);
    vec3 lightPoint = vec3(1, 1, 1);
    
    vec3 lightVec = lightPoint - wPos;
    
	gl_Position = P * V* MV * vertPos;
    
    if(followRot == 0)
        fragNor = (V * MV * vec4(vertNor, 0.0)).xyz;
    else if(followRot == 1)
        fragNor = vertNor;
    
    diffShade = max(dot(fragNor, normalize(lightVec)), 0) * Kd;
    ambient = Kd * .2;
}
