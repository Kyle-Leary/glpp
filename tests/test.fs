#include "test_common.glinc"

out vec4 FragColor;

void main() {
    float angle = PI / 4.0;
    FragColor = vec4(sin(angle), cos(angle), 0.0, 1.0);
}
