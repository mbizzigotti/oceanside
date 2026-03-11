#pragma once
#include "camera.h"
#include "object.h"
#include "graphics.h"
#include "3rdparty/stb_image.h"

struct Scene {
    Camera camera;
    Skybox skybox;
    std::vector<Object> objects;

    Result load(string filename, Graphics &gfx);
    void update_and_render(Graphics &gfx, float dt);
};
