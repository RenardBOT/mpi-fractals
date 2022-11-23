#version 430


layout(location = 0) in vec3 position; // le location permet de dire de quel flux/canal on récupère la données
layout(location = 1) in vec3 color; // le location permet de dire de quel flux/canal on récupère la données
uniform vec3 inputColor;
out vec3 fragColor;

uniform mat4 MVP;


void main(){
        gl_Position = MVP * vec4(position, 1.0);
        fragColor = color;
}


