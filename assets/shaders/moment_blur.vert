#version 330 core

out vec2 texCoord;

void main()
{
    // Fullscreen triangle positions in clip space.
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    // Reuse the same triangle to cover the whole render target without a vertex buffer.
    vec2 pos = positions[gl_VertexID];
    texCoord = pos * 0.5 + 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}
