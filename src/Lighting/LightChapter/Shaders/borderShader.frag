#version 120


uniform vec3 borderColor;

void main() {
    gl_FragColor = vec4(borderColor, 1.0);
}
