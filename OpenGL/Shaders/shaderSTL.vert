#version 410

in vec3 position;
//in vec3 color;

//out vec3 outputColor;

void main()
{
    gl_Position = vec4(position * .01, 1.0);
    //    outputColor = color;
}
