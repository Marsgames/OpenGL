#version 410

in vec3 position;
in vec3 color;

//vec3 fadeOut = vec3{1, 1, 1};

out vec3 outputColor;

void main()
{
    gl_Position = vec4(position, 1.0);
    outputColor = color;
}
