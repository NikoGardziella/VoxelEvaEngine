#type vertex
#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 2) in uint aPickID;

layout(location = 0) flat out uint vPickID;

layout(push_constant) uniform PushConstants
{
    mat4 ViewProjection;
} pc;

void main()
{
    vPickID = aPickID;
    gl_Position = pc.ViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 450

layout(location = 0) flat in uint vPickID;
layout(location = 0) out uint outPickID;

void main()
{
    outPickID = vPickID;
}