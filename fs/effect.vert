#version 150�@core
 
layout (location = 0) in vec4 pv;
 
void main(void)
{
    gl_Position = pv;
}
