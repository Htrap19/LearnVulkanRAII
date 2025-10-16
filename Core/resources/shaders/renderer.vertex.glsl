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

layout (set = 0, binding = 0) uniform SceneUniformData
{
    CameraViewData cameraViewData;
    ObjectMetadata objectsMetadata[32];
} u_sceneUniformData;

void main()
{
    mat4 proj = u_sceneUniformData.cameraViewData.projection;
    mat4 view = u_sceneUniformData.cameraViewData.view;
    mat4 model = u_sceneUniformData.objectsMetadata[0].model;

    gl_Position = proj * view * model * vec4(a_pos, 1.0);
}
