#version 420 core

in vec2 v_position;
in vec2 v_light_position;
in vec3 v_light_color;
in vec4 v_attenuation;

#define k_quadratic v_attenuation.x
#define k_linear v_attenuation.y
#define k_const v_attenuation.z
#define k_bias v_attenuation.w

out vec3 frag_color;

void main() 
{
    // distance to light
    float d = distance(v_position, v_light_position);

    frag_color = max(
        vec3(0.0), 
        v_light_color / (k_quadratic*d*d + k_linear*d + k_const) - vec3(k_bias)
    );
}