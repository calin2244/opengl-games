#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float elpsTime;
uniform vec3 u_color;

// Output
out vec3 frag_color;

void main()
{   
    frag_color = u_color;

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);   
}
