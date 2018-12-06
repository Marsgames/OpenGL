#version 410

out vec4 color;
in vec3 outputColor;

void main()
{
    color = vec4(outputColor, 1);
//    color = vec4(0, 1, 0, 1);
}
