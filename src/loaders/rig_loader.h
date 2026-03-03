#pragma once
#include "object_loader.h"

struct RigObjectLoader : public ObjectLoader {
    string filename;

public:
    RigObjectLoader(cJSON *object);

    // --- Object Loader required functions ---

    const char* name() override { return "Rig"; }
    Result load(string scene_folder, Graphics &gfx, Object *object) override;
    Result write_buffers(Graphics &gfx) override;
};
