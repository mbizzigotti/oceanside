#include "shared.inl"
#define FONT_SCALE 2.0

struct VSOut {
    float4 position : SV_Position;
    float4 param1;
    float4 param2;
};

[[vk::push_constant]] ConstantBuffer<PerDraw> draw;
[[vk::binding(0)]] ConstantBuffer<Scene> scene;
[[vk::binding(1)]] ByteAddressBuffer vertex_buffer;
// [[vk::binding(2)]] StructuredBuffer<PointLight> lights;
// [[vk::binding(3)]] StructuredBuffer<float4x4> joint_transforms;
[[vk::binding(2)]] SamplerCube skybox;

[shader("vertex")]
VSOut main(uint vertexIndex : SV_VertexID)
{
    VSOut output;

    switch (draw.shader_id) {
    case Shader.Position_Normal: {
        const uint index = vertexIndex * sizeof(PNVertex) + draw.vertex_offset;
        PNVertex vertex = vertex_buffer.LoadByteOffset<PNVertex>(index);
        float4 world_position = mul(draw.model, float4(vertex.position, 1));
        output.position = mul(scene.view_proj, world_position);
        output.param1 = world_position;
        output.param2 = float4(vertex.normal, 0);
    } break;

    case Shader.Text: {
        const uint index = vertexIndex * sizeof(TextVertex) + draw.vertex_offset;
        TextVertex vertex = vertex_buffer.Load<TextVertex>(index);
        float2 scale = FONT_SCALE / scene.screen_size;
        output.position = float4(vertex.position.xy * scale - 1, 0, 1);
        output.param1 = unpackUnorm4x8ToFloat(vertex.color);
    } break;

    case Shader.Skybox: {
        /*
            0 -- 1
            |\   |\
            | 4 -- 5
            | |  | |
            2-|- 3 |
             \|   \|
              6 -- 7
            
            z y
            \|
             *- x
        */

        float3 cube_vertices[] = {
            {-1, 1, 1}, { 1, 1, 1}, {-1,-1, 1}, { 1,-1, 1},
            {-1, 1,-1}, { 1, 1,-1}, {-1,-1,-1}, { 1,-1,-1}
        };

        uint cube_indices[] = {
            0, 1, 3, 0, 3, 2, // Z+
            1, 0, 4, 1, 4, 5, // Y+
            0, 2, 6, 0, 6, 4, // X-
            1, 5, 7, 1, 7, 3, // X+
            2, 3, 7, 2, 7, 6, // Y-
            4, 6, 7, 4, 7, 5, // Z-
        };

        uint index = cube_indices[vertexIndex];
        float3 P = cube_vertices[index];
        output.param1 = float4(P, 1);
        float4 L = mul(scene.view_proj, float4(512.0 * P, 1.0));
        output.position = float4(L.xy, L.w - 0.001, L.w);
    }
    }
    return output;
}

float3 eval_point_light(PointLight light, float3 position, float3 normal)
{
    // Ambient
    float3 ambient = light.ambient_strength * light.color;

    // Diffuse
    float3 dir_to_light = normalize(light.position - position);
    float diff = max(dot(normal, dir_to_light), 0);
    float3 diffuse = diff * light.color;

    // Specular
    float3 viewDir = normalize(scene.view_pos - position);
    float3 reflectDir = reflect(-dir_to_light, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 32);
    float3 specular = light.specular_strength * spec * light.color;

    return ambient + diffuse + specular;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    switch (draw.material_id) {
    case Material.Phong: {
        PointLight light = {
            { 0, 6, 10 }, { 1, 1, 1 }, 0.1, 0.5,
        };
        float3 normal = normalize(input.param2.xyz);
        float3 position = input.param1.xyz;
        float3 color = 0;//eval_point_light(light, position, normal);

        float3 viewDir = normalize(scene.view_pos - position);
        float3 reflectDir = reflect(-viewDir, normal);
        float spec = pow(max(dot(viewDir, -reflectDir), 0), 1);
        float3 Le = skybox.Sample(reflectDir).rgb;
        float3 specular = lerp(float3(0.1), Le, 1 - spec);
        color += specular;

        return float4(color, 1);
    }

    case Material.Text: {
        return input.param1;
    }

    case Material.Skybox: {
        return skybox.Sample(input.param1.xyz);
    }
    }
    return float4(0);
}
