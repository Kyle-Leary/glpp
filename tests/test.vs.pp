#version 330

#define PI 3.14159265

vec3 computeNormal(vec3 position) {
    return normalize(cross(dFdx(position), dFdy(position)));
}

layout (location = 0) in vec3 aPos;

void main() {
    vec3 normal = computeNormal(aPos);
    gl_Position = vec4(aPos, 1.0);
}
