
#ifdef __cplusplus
#include "../core.h"
namespace shader {
using float2   =  vec2;
using float3   =  vec3;
using float4   =  vec4;
using uint2    = uvec3;
using uint4    = uvec4;
using uint3    = uvec3;
using float4x4 =  mat4;
#endif

enum class Shader {
    Position_Normal,
    Text,
    Skybox,
};

enum class Material {
    Phong,
    Text,
    Skybox,
};

struct PerDraw {
    float4x4 model;
    uint vertex_offset;
    Shader shader_id;
    Material material_id;
};

struct Scene {
    float4x4 view_proj;
	float3   view_pos;
	float    _padding;
    uint2    screen_size;
	uint     light_count;
};

struct TextVertex {
    float3 position;
    uint   color;
};

struct PNVertex {
    float3 position;
    float3 normal;
};

struct PointLight {
	float3 position;
	float3 color;
    float ambient_strength;
    float specular_strength;
};

#ifdef __cplusplus
}
#endif
