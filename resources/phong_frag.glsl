#version 330 core 
out vec4 color;

in vec3 fragNor;//'N'
in vec3 worldPos;

uniform float lightPos;//shift light in x direction
uniform mat4 V;

uniform vec3 MatDif;//Kd
uniform vec3 MatAmb;//Ka
uniform vec3 MatSpec;//Ks
uniform float shine;//n

void main()
{
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 Lc = vec3(1, 1, 1);//light color
    vec3 Vvec = -1 * worldPos;
    
    //**if multiply lightPoint by Vvec it acts as a flashlight
    //**if remove V multiplication light will be relative to current cameraview
    //multiply light position by V matrix to have it fixed in world
    vec3 lightPoint = (V * vec4(vec3(3+lightPos, 10, 2), 0)).xyz;//try on CPU
    vec3 L = lightPoint - worldPos;
    
    vec3 R = normalize(-L) + (2*dot(normalize(L),normalize(fragNor))*normalize(fragNor));

    diffuse =  MatDif * max(dot(normalize(fragNor), normalize(L)), 0) * Lc;
    specular = MatSpec * pow( max(0, dot(normalize(Vvec), normalize(R))), shine) * Lc;
    ambient = MatAmb * Lc;

    
    color = vec4(diffuse,1) + vec4(specular,1) + vec4(ambient,1);
}
