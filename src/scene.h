#pragma once
#include "camera.h"
#include "object.h"

struct Graphics;

struct Skybox {
    void render(Graphics &gfx) { (void)gfx; }
};

struct GPU_Scene {
    // Camera view-projection matrix
    mat4 view_proj;
};

struct Scene {
    Camera camera;
    Skybox skybox;
    std::vector<Object> objects;

    Result load(string filename, Graphics &gfx);
    void update_and_render(Graphics &gfx, float dt);
};
