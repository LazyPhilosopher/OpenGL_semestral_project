#version 330 core

out vec4 FragColor;

void main()
{
    // Determine the axis based on the vertex position
    vec3 color;
    if (gl_FragCoord.y == 0.0)
    {
        // X-axis: Red
        color = vec3(1.0, 0.0, 0.0);
    }
    else if (gl_FragCoord.x == 0.0)
    {
        // Y-axis: Green
        color = vec3(0.0, 1.0, 0.0);
    }
    else if (gl_FragCoord.z == 0.0)
    {
        // Z-axis: Blue
        color = vec3(0.0, 0.0, 1.0);
    }

    // Output color
    FragColor = vec4(color, 1.0);
}
