#include "common.inl"

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

[shader("vertex")]
VSOut main(uint vertexIndex : SV_VertexID)
{
    VSOut output;

    switch (draw.shader_id) {
    case Shader.Position_Normal: {
        const uint index = vertexIndex * sizeof(PNVertex) + draw.vertex_offset;
        PNVertex vertex = vertex_buffer.LoadByteOffset<PNVertex>(index);
        float4 world_position = mul(draw.model, float4(vertex.position, 1.0));
        output.position = mul(scene.view_proj, world_position);
        output.param1 = world_position;
        output.param2 = float4(vertex.normal, 0.0);
    } break;

    case Shader.Text: {
        const uint index = vertexIndex * sizeof(TextVertex) + draw.vertex_offset;
        TextVertex vertex = vertex_buffer.Load<TextVertex>(index);
        float2 scale = 4.0 / scene.screen_size;
        output.position = float4(vertex.position.xy * scale - 1.0, 0, 1);
        output.param1 = unpackUnorm4x8ToFloat(vertex.color);
    } break;
    }
    return output;
}

float3 eval_point_light(PointLight light, float3 position, float3 normal)
{
    // Ambient
    float ambientStrength = 0.1;
    float3 ambient = ambientStrength * light.color;

    // Diffuse
    float3 dir_to_light = normalize(light.position - position);
    float diff = max(dot(normal, dir_to_light), 0.0);
    float3 diffuse = diff * light.color;

    // Specular
    float specularStrength = 0.5;
    float3 viewDir = normalize(scene.view_pos - position);
    float3 reflectDir = reflect(-dir_to_light, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = specularStrength * spec * light.color;

    return ambient + diffuse + specular;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    switch (draw.material_id) {
    case Material.Phong: {
        PointLight light = {
            { 0, 6, 10 },
            { 1, 1, 1 },
        };

        float3 normal = normalize(input.param2.xyz);
        float3 position = input.param1.xyz;
        float3 color = eval_point_light(light, position, normal);
        return float4(color, 1.0);
    }

    case Material.Text: {
        return input.param1;
    }
    }
    return float4(0.0);
}
