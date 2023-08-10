#version 330

#include "test_common.glinc"

layout (location = 0) in vec3 aPos;

void main() {
    vec3 normal = computeNormal(aPos);
    gl_Position = vec4(aPos, 1.0);
}
