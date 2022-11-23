#version 430


out vec4 finalColor;
in vec3 fragColor;

void main() {
   finalColor = vec4(fragColor,1.);
}
