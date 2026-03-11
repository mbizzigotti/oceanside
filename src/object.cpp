#include "object.h"
#include "scene.h"
#include "graphics.h"

void Object::update_and_render(Scene& scene, Graphics& gfx, float dt) {
    // not used
    (void)scene;
    (void)dt;

    VkCommandBuffer cmd = gfx.current_frame.command_buffer;
    VkPipelineLayout layout = gfx.pipeline_layout;

    switch (type) {
    case Type::Mesh: {
        shader::PerDraw draw = {
            .model = mesh.model,
            .vertex_offset = mesh.vertex_offset,
        };
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(draw), &draw);
        vkCmdDrawIndexed(cmd, mesh.index_count, 1, mesh.index_start, 0, 0);
    } break;
    
    default: break;
    }
}

void Skybox::render(Graphics &gfx) {
    VkCommandBuffer cmd = gfx.current_frame.command_buffer;
    VkPipelineLayout layout = gfx.pipeline_layout;

    shader::PerDraw draw = {
        .shader_id = shader::Shader::Skybox,
        .material_id = shader::Material::Skybox,
    };
    vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(draw), &draw);
    vkCmdDraw(cmd, 36, 1, 0, 0);
}
