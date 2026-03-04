#pragma once
#include "object_loader.h"

struct Tokenizer;

struct MeshObjectLoader : public ObjectLoader {
    using Vertex = shader::PNVertex;

    string              filename;
    mat4                model;
    uint32_t            vertex_offset;
    uint32_t            index_offset;
    std::vector<Vertex> vertices;
    std::vector<u32>    indices;

public:
    MeshObjectLoader(cJSON *object);

    // --- Object Loader required functions ---

    const char* name() override { return "Mesh"; }
    Result load(string scene_folder, Graphics &gfx, Object *object) override;
    Result write_buffers(Graphics &gfx) override;

private:
    Result load_from_file(string filename);

    struct Obj_LoaderContext {
        std::unordered_map<uvec3, u32> vertex_map;
        std::vector<vec3> positions;
        std::vector<vec2> texcoords;
        std::vector<vec3> normals;
        u32 next_index = 0;
    };

    Result parse_from_obj(string filename, string contents);
    Result parse_line_obj(Tokenizer &tokenizer, Obj_LoaderContext &context);
};
