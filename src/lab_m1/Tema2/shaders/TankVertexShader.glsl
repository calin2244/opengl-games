#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 vertex_normal;

// Uniforms
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 u_color;
uniform float u_tankHealth;
uniform bool u_isTankBody;

// Output
out vec3 frag_color;

// Will this even look cool :(
float calculateDeformation(float position, float health) {
    float magnitude = 0.1; 
    float frequency = 5.0; 
    return sin(position * frequency + health) * magnitude;
}

void main()
{   
    frag_color = u_color;
    vec3 position = v_position;

    if(u_tankHealth <= 50 && u_isTankBody){
        float deformationY = calculateDeformation(position.y, u_tankHealth);
        position.y += deformationY;
        
        float deformationX = calculateDeformation(position.x, u_tankHealth);
        position.x += deformationX;
        
        float deformationZ = calculateDeformation(position.z, u_tankHealth);
        position.z += deformationZ;
    }

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
