#version 450

layout (location = 0) in vec3 a_pos;

layout (set = 0, binding = 0) uniform CameraViewData
{
    mat4 projection;
    mat4 view;
} u_cameraViewData;

void main()
{
    gl_Position = u_cameraViewData.projection * u_cameraViewData.view * vec4(a_pos, 1.0);
}
