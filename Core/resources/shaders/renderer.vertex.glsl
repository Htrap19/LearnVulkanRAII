#version 450

struct CameraViewData
{
    mat4 projection;
    mat4 view;
};

struct ObjectMetadata
{
    mat4 model;
};

layout (location = 0) in vec3 a_pos;
layout (location = 1) in int a_objectMetadataIndex;

layout (set = 0, binding = 0) uniform CameraViewDataBuffer
{
    CameraViewData cameraViewData;
} u_cameraViewDataBuffer;

layout (std140, set = 0, binding = 1) readonly buffer ObjectMetadataBuffer
{
    ObjectMetadata metadata[];
} b_objectMetadataBuffer;

void main()
{
    mat4 proj = u_cameraViewDataBuffer.cameraViewData.projection;
    mat4 view = u_cameraViewDataBuffer.cameraViewData.view;
    mat4 model = b_objectMetadataBuffer.metadata[a_objectMetadataIndex].model;

    gl_Position = proj * view * model * vec4(a_pos, 1.0);
}
