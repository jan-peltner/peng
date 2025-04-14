#version 330

in vec3 vertexPosition;
in vec4 vertexColor;

uniform vec2 uResolution;

out vec4 fragColor;

void main() {
    // normalize vertex coords to [-1, 1]
    vec2 ndc = (vertexPosition.xy / uResolution) * 2 - 1;
    ndc.y = -ndc.y;
    fragColor = vertexColor;
    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = 10.0; 
}
