#include "scene.h"
#include "graphics.h"
#include "3rdparty/cJSON.h"
#include "loaders/object_loader.h"

Result Scene::load(string filename, Graphics &gfx) {
    optional<string> scene_text = read_entire_file(filename);
    if (!scene_text)
        return ERROR("Failed to load scene file \"%.*s\"", FORMAT_STRING(filename));

    AutoFree<string> contents { scene_text.value() };
    cJSON *json = cJSON_ParseWithLength(contents.data(), contents.size());
    if (!json)
        return ERROR("Failed to parse scene file!");

    Result result = Success;
    string parent = parent_directory(filename);

    camera = Camera::get_default();
    camera.load(cJSON_GetObjectItem(json, "Camera"));

    std::vector<std::unique_ptr<ObjectLoader>> loaders;
    if (cJSON *object_array = cJSON_GetObjectItem(json, "Objects")) {
        // Initialize our list of objects
        objects.resize(cJSON_GetArraySize(object_array));
        memset(objects.data(), 0, sizeof(Object) * objects.size());

        // Load each object
        int i = 0;
        cJSON *object = 0;
        cJSON_ArrayForEach(object, object_array) {
            if (auto loader = create_object_loader(object)) {
                if ((result = loader->load(parent, gfx, &objects[i]))) {
                    ERROR("Failed to load object from %s object loader!", loader->name());
                    goto error;
                }
                loaders.emplace_back(std::move(loader));
            }
            i += 1;
        }
    }

    gfx.allocate_required_memory();

    for (auto& loader: loaders) {
        loader->write_buffers(gfx);
    }

error:
    cJSON_Delete(json);
    return result;
}

void Scene::update_and_render(Graphics &gfx, float dt) {
    if (RGFW_isKeyPressed(RGFW_tab))
        camera.type = Camera::Type(((int)camera.type + 1) % 2);

    camera.update(dt);
    for (auto &obj: objects) {
        obj.update_and_render(gfx.current_frame.command_buffer, gfx.pipeline_layout, dt);
    }
    skybox.render(gfx);
	
    gfx.scene_data.view_proj = camera.get_transform(&gfx.scene_data.view_pos);
    gfx.scene_data.screen_size.x = gfx.image_extent.width;
    gfx.scene_data.screen_size.y = gfx.image_extent.height;
}
